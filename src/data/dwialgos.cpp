/*
 * dwialgos.cpp
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */
#include <stdexcept>

#include <math.h>

#include <QtCore/QDebug>
#include <QtCore/QVector>
#include <QtGui/QVector3D>

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "mesh/trianglemesh.h"
#include "mesh/tesselation.h"

#include "datasetdwi.h"

#include "dwialgos.h"


DWIAlgos::DWIAlgos()
{
}

DWIAlgos::~DWIAlgos()
{
}

DatasetDWI* DWIAlgos::qBall( DatasetDWI* ds )
{
    qDebug() << "start calculating qBall";
    QVector<QVector3D>bvecs =  ds->getBvecs();

    Matrix gradients( bvecs.size(), 3 );
    for ( int i = 0; i < bvecs.size(); ++i )
    {
        gradients( i+1, 1 ) = bvecs.at( i ).x();
        gradients( i+1, 2 ) = bvecs.at( i ).y();
        gradients( i+1, 3 ) = bvecs.at( i ).z();
    }

    double lambda = 0.006;
    int maxOrder = 4;
    Matrix qBallBase = calcQBallBase( gradients, lambda, maxOrder );

    qDebug() << "elements in data" << ds->getDataVector()->at( 0 ).Nrows();
    qDebug() << "elements in qball base" << qBallBase.Nrows() << " " << qBallBase.Ncols();

    QVector<ColumnVector>* data = ds->getDataVector();

    QVector<ColumnVector>* qballVector = new QVector<ColumnVector>();

    for ( int i = 0; i < data->size(); ++i )
    {
        qballVector->push_back( qBallBase * data->at( i ) );
    }

    DatasetDWI* out = new DatasetDWI( ds->getProperty( "fileName" ).toString(), qballVector, ds->getB0Data(), ds->getBvals(), bvecs );
    out->parseNiftiHeader( ds->getHeader() );
    out->examineDataset();
    out->setProperty( "fileName", "QBall" );
    out->setProperty( "name", "QBall" );

    qDebug() << "finished calculating qBall";

    return out;
}

Matrix DWIAlgos::calcQBallBase( Matrix gradients, double lambda, int maxOrder )
{
    qDebug() << "start calculating qBall base";
    double sh_size( ( maxOrder + 1 ) * ( maxOrder + 2 ) / 2 );

    // check validity of input:
    if ( gradients.Nrows() == 0 )
        throw std::invalid_argument( "No gradient directions specified." );

    if ( gradients.Ncols() != 3 )
        throw std::invalid_argument( "Gradients have to be 3D." );

    // calculate spherical harmonics base:
    Matrix SH = sh_base( gradients, maxOrder );

    // calculate the Laplace-Beltrami and the Funk-Radon transformation:
    ColumnVector LBT( sh_size );
    ColumnVector FRT( sh_size );

    for ( int order = 0; order <= maxOrder; order += 2 )
    {
        double frt_val = 2.0 * M_PI * boost::math::legendre_p<double>( order, 0 );
        double lbt_val = lambda * order * order * ( order + 1 ) * ( order + 1 );

        for ( int degree( -order ); degree <= order; degree++ )
        {
            int i = order * ( order + 1 ) / 2 + degree;
            LBT( i+1 ) = lbt_val;
            FRT( i+1 ) = frt_val;
        }
    }

    // prepare the calculation of the pseudoinverse:
    Matrix B = SH.t() * SH;

    // update with Laplace-Beltrami operator:
    for( int i = 0; i < sh_size; i++ )
    {
        B(i+1,i+1) += LBT(i+1);
    }

    Matrix out = B.i() * SH.t();

    // the Funk-Radon transformation:
    for ( unsigned long i = 0; i < B.Nrows(); i++ )
    {
        for ( unsigned long j = 0; j < B.Ncols(); j++ )
        {
            out( i+1, j+1 ) *= FRT( i+1 );
        }
    }

    qDebug() << "finished calculating qBall base";

    return out;
}


Matrix DWIAlgos::sh_base( Matrix g, int maxOrder )
{
    qDebug() << "start calculating sh base";
  // allcoate result matrix
    unsigned long sh_dirs( ( maxOrder + 2 ) * ( maxOrder + 1 ) / 2 );
    Matrix out( g.Nrows(), sh_dirs );

    // for each direction
    for ( unsigned long i=0; i < g.Nrows(); i++ )
    {
        // transform current direction to polar coordinates
        double theta( acos( g( i+1, 3 ) ) );
        double phi( atan2( g( i+1, 2 ), g( i+1, 1 ) ) );

        // calculate spherical harmonic base
        for ( int order = 0, j=0; order <= maxOrder; order += 2 )
        {
            for ( int degree( -order ); degree <= order; degree++, j++ )
            {
                out( i+1, j+1 ) = sh_base_function( order, degree, theta, phi );
            }
        }
    }
    qDebug() << "finished calculating sh base";
    return out;
}

double DWIAlgos::sh_base_function( int order, int degree, double theta, double phi )
{
    using namespace boost::math;

    if ( degree > 0 )
    {
        return spherical_harmonic_r( order, abs( degree ), theta, phi );
    }
    else if ( degree < 0 )
    {
        return spherical_harmonic_i( order, abs( degree ), theta, phi );
    }
    else
    {
        return spherical_harmonic_r( order, 0, theta, phi );
    }
}

TriangleMesh* DWIAlgos::writeSHGlyph( DatasetDWI* ds, unsigned int steps, double color )
{
    TriangleMesh* out = new TriangleMesh();

    //get tesselation:
    const Matrix* sphere_v( tess::vertices( steps ) );

    QVector<ColumnVector>* data = ds->getDataVector();
    const int order( ( -3 + ( int ) ( sqrt( 8 * ( data->at(0).Nrows() ) + 1 ) ) ) / 2 );

    // calculate sh_base: (could be optimized via lookup table)
    Matrix base( sh_base( *sphere_v, order ) );

    QVector3D dist;
    dist.setX( ds->getProperty( "dx" ).toFloat() );
    dist.setY( ds->getProperty( "dy" ).toFloat() );
    dist.setZ( ds->getProperty( "dz" ).toFloat() );

    // calculate size parameters:
    unsigned long num( 0 );

    // compute glyph at every voxel:
    for ( int x = 0; x < ds->getProperty( "nx" ).toInt(); ++x )
    {
        for ( int y = 0; y < ds->getProperty( "ny" ).toInt(); ++y )
        {
            for ( int z = 0; z < ds->getProperty( "nz" ).toInt(); ++z )
            {
                if ( ds->getDataVector()->at( 0 )( 0 ) != 0.0 )
                {
                    // calculate all the radii in the voxel and scale them:
                    ColumnVector radius = base * ds->getDataVector()->at( 0 );
                    radius = radius / 4.0;

                    for ( int i = 0; i < radius.Nrows(); ++i )
                    {
                        // remove negative lobes:
                        if ( radius( i ) < 0 )
                        {
                            radius( i ) = 0.0;
                        }
                        QVector3D v;
                        v.setX( (double)( radius( i ) * ( ( *sphere_v )( i, 1 ) + .5 + x ) * dist.x() ) );
                        v.setY( (double)( radius( i ) * ( ( *sphere_v )( i, 2 ) + .5 + y ) * dist.y() ) );
                        v.setZ( (double)( radius( i ) * ( ( *sphere_v )( i, 3 ) + .5 + z ) * dist.z() ) );
                        out->addVertex( v );
                    }
                }
            }
        }
    }

    const int* faces( tess::faces( steps ) );

    for ( int i = 0; i < tess::n_faces( steps); ++i )
    {
        Triangle tri = { faces[3*i], faces[3*i+1], faces[3*i+2] };
        out->addTriangle( tri );
    }

    // set coloring by setting magnitudes:
//    fprintf( out_f, "Magnitudes\n" );
//    if ( color < 0.0 )
//        for ( unsigned long i( 0 ); i < num; i++ )
//            for ( unsigned long j( 0 ); j < ( *sphere_v ).size1(); j++ )
//            {
//                fprintf( out_f, "%.8f\n", mri_color( ( *sphere_v )( j, 0 ), ( *sphere_v )( j, 1 ), ( *sphere_v )( j, 2 ) ) );
//            }
//    else
//        for ( unsigned long i( 0 ); i < num; i++ )
//            for ( unsigned long j( 0 ); j < ( *sphere_v ).size1(); j++ )
//            {
//                fprintf( out_f, "%.8f\n", color );
//            }

}
