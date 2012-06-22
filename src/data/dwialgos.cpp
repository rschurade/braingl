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

#include "datasetdwi.h"

#include "dwialgos.h"

DWIAlgos::DWIAlgos()
{
}

DWIAlgos::~DWIAlgos()
{
}

DatasetNifti* DWIAlgos::qBall( DatasetDWI* ds )
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
    qDebug() << "finished calculating qBall";

    switch ( ds->getProperty( "datatype" ).toInt() )
    {
        case NIFTI_TYPE_UINT8:
        {
            uint8_t* data;
            data = reinterpret_cast< uint8_t* >( ds->getData() );
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            int16_t* data;
            data = reinterpret_cast< int16_t* >( ds->getData() );
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            int32_t* data;
            data = reinterpret_cast< int32_t* >( ds->getData() );
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        {
            float* data;
            data = reinterpret_cast< float* >( ds->getData() );
            break;
        }
        case NIFTI_TYPE_INT8:
        {
            int8_t* data;
            data = reinterpret_cast< int8_t* >( ds->getData() );
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            uint16_t* data;
            data = reinterpret_cast< uint16_t* >( ds->getData() );
            break;
        }
    }
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

//template<typename T> void DWIAlgos::multQBall( T* data, DatasetDWI* ds, boost::shared_ptr<Matrix> qBallBase )
//{
//    int nx = ds->getProperty( "nx" ).toInt();
//    int ny = ds->getProperty( "ny" ).toInt();
//    int nz = ds->getProperty( "nz" ).toInt();
//
//    int size = nx * ny * nz;
//
//    for ( int i = 0; i < size; ++i )
//    {
//       Vector v();
//
//    }
//}
