/*
 * fibers.cpp
 *
 * Created on: Jan 24, 2013
 * @author Ralph Schurade
 */
#include "fibers.h"

#include "../data/datasets/datasetfibers.h"
#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/dataset3d.h"
#include "../data/models.h"

#include "../algos/colormapbase.h"
#include "../gui/gl/colormapfunctions.h"

#include <QSet>
#include <QVector3D>

#include <limits>
#include <math.h>

#define kPI 3.14159265358979323846264338327950

Fibers::Fibers( DatasetFibers* ds ) :
    m_dataset( ds ),
    m_nx( 1 ),
    m_ny( 1 ),
    m_nz( 1 ),
    m_dx( 1.0 ),
    m_dy( 1.0 ),
    m_dz( 1.0 ),
    m_ax( 0.0 ),
    m_ay( 0.0 ),
    m_az( 0.0 ),
    m_blockSize( 1 )
{
}

Fibers::~Fibers()
{
}

DatasetFibers* Fibers::thinOut()
{
    float maxDistance = 1.0;
    bool merged = true;
    bool currentMerged = false;
    int iteration = 1;
    std::vector<Fib> fibs = *( m_dataset->getFibs() );
    std::vector<Fib> mergedFibs;
    std::vector<Fib> unmergedFibs;

    while ( merged )
    {
        qDebug() << "iteration:" << iteration;
        ++iteration;
        qDebug() << fibs.size() << "fibers before merging";

        merged = false;
        std::vector<bool>processed( fibs.size(), false );

        for ( unsigned int i = 0; i < fibs.size(); ++i )
        {
            currentMerged = false;
            Fib currentFib = fibs.at( i );
            for( unsigned int k = i + 1; k < fibs.size(); ++k )
            {
                if ( currentFib.length() == fibs.at( k ).length() )
                {
                    float dist = currentFib.dist( fibs.at( k ) );
                    if ( dist < maxDistance )
                    {
                        mergedFibs.push_back( mergeFibs( currentFib, fibs.at( k ) ) );
                        merged = true;
                        currentMerged = true;
                        processed[k] = true;
                        break;
                    }
                }
            }
            if ( !currentMerged && !processed[i] )
            {
                unmergedFibs.push_back( currentFib );
            }
        }
        fibs = mergedFibs;
        mergedFibs.clear();
        qDebug() << fibs.size() + unmergedFibs.size() << "fibers after merging";
    }
    fibs.insert( fibs.end(), unmergedFibs.begin(), unmergedFibs.end() );
    return new DatasetFibers( QDir( "new fibers" ), fibs, m_dataset->getDataNames() );
}

Fib Fibers::mergeFibs( Fib& lhs, Fib& rhs )
{
    Fib out;
    for ( unsigned int i = 0; i < lhs.length(); ++i )
    {
        out.addVert( ( lhs[i] + rhs[i] ) / 2 );
        out.setData( 0, i, ( lhs.getData( 0, i ) + rhs.getData( 0, i ) ) / 2 );
    }

    for ( unsigned int i = 1; i < lhs.getCountDataFields(); ++i )
    {
        out.addDataField();
        for ( unsigned int k = 0; k < lhs.length(); ++k )
        {
            out.setData( i, k, ( lhs.getData( 0, k ) + rhs.getData( 0, k ) ) / 2 );
        }
    }

    return out;
}

DatasetScalar* Fibers::tractDensity()
{
    float res = Models::getGlobal( Fn::Property::G_TRACT_TEX_RESOLUTION ).toFloat();

    QVector3D lowerCorner = m_dataset->getBoundingBox().first;
    QVector3D upperCorner = m_dataset->getBoundingBox().second;
    m_dx = res;
    m_dy = res;
    m_dz = res;
    m_ax = lowerCorner.x();
    m_ay = lowerCorner.y();
    m_az = lowerCorner.z();
    m_nx = ( upperCorner.x() - m_ax ) / res;
    m_ny = ( upperCorner.y() - m_ay ) / res;
    m_nz = ( upperCorner.z() - m_az ) / res;

    m_blockSize = m_nx * m_ny * m_nz;
    std::vector<float> data( m_blockSize, 0 );
    std::vector<Fib> fibs = m_dataset->getSelectedFibs();

    float x, y, z;
    QVector3D p3;
    qDebug() << "calculating tract density for " << fibs.size() << " fibers...";
    for ( unsigned int i = 0; i < fibs.size(); ++i )
    {
        if ( i % 1000 == 0 ) qDebug() << i << " done...";
        std::vector<float> fibdata( m_blockSize, 0 );

        for( unsigned int k = 0; k < fibs[i].length() - 1; ++k )
        {
            QVector3D p1( fibs[i][k] );
            QVector3D p2( fibs[i][k+1] );

            for ( int l = 0; l < 10; ++l )
            {
                p3 = ( 1.0 - ( (float)l / 10. ) ) * p1 + ( (float)l / 10. ) * p2;
                x = p3.x();
                y = p3.y();
                z = p3.z();

                int id = getID( x, y, z );

                if ( fibdata[id] == 0 )
                {
                    ++fibdata[id];
                    ++data[id];
                }
            }
        }
    }

    nifti_image* header = createHeader( 1 );
    DatasetScalar* out = new DatasetScalar( QDir( "tract density" ), data, header );
    return out;
}

Dataset3D* Fibers::tractColor()
{
    QVector3D lowerCorner = m_dataset->getBoundingBox().first;
    QVector3D upperCorner = m_dataset->getBoundingBox().second;
    m_dx = Models::getGlobal( Fn::Property::G_TRACT_TEX_RESOLUTION ).toFloat();
    m_dy = m_dx;
    m_dz = m_dx;
    m_ax = lowerCorner.x();
    m_ay = lowerCorner.y();
    m_az = lowerCorner.z();
    m_nx = ( upperCorner.x() - m_ax ) / m_dx;
    m_ny = ( upperCorner.y() - m_ay ) / m_dx;
    m_nz = ( upperCorner.z() - m_az ) / m_dx;

    int source = Models::getGlobal( Fn::Property::G_TRACT_TEXT_SOURCE ).toInt();
    m_blockSize = m_nx * m_ny * m_nz;
    std::vector<QVector3D> data( m_blockSize );
    std::vector<int> count( m_blockSize, 0 );
    std::vector<Fib> fibs = m_dataset->getSelectedFibs();

    QSet<int> visited;
    int id = 0;

    switch ( source )
    {
        case 0:
        {
            for ( unsigned int i = 0; i < fibs.size(); ++i )
            {
                Fib fib = fibs[i];
                visited.clear();

                for ( unsigned int k = 0; k < fib.length(); ++k )
                {
                    QVector3D localColor = fib.getTangent( k );
                    localColor.setX( fabs( localColor.x() ) );
                    localColor.setY( fabs( localColor.y() ) );
                    localColor.setZ( fabs( localColor.z() ) );
                    localColor.normalize();

                    id = getID( fib[k].x(), fib[k].y(), fib[k].z() );
                    if ( !visited.contains( id) )
                    {
                        ++count[ id ];
                        data[ id ] += localColor;
                        visited.insert( id );
                    }
                }
            }

            for ( int i = 0; i < m_blockSize; ++i )
            {
                data[i] /= count[i];
                count[i] = qMin( count[i], 10 );
            }
            float div;
            for ( int i = 0; i < m_blockSize; ++i )
            {
                div = ( (float)count[i] / 20. ) + 0.5;
                data[i] *= div;
            }
            break;
        }
        case 1:
        {
            for ( unsigned int i = 0; i < fibs.size(); ++i )
            {
                Fib fib = fibs[i];
                visited.clear();

                QColor localColor = fib.globalColor();
                QVector3D colVec( localColor.redF(), localColor.greenF(), localColor.blue() );

                for ( unsigned int k = 0; k < fib.length(); ++k )
                {
                    id = getID( fib[k].x(), fib[k].y(), fib[k].z() );
                    if ( !visited.contains( id) )
                    {
                        ++count[ id ];
                        data[ id ] += colVec;
                        visited.insert( id );
                    }
                }
            }
            for ( int i = 0; i < m_blockSize; ++i )
            {
                data[i] /= count[i];
                count[i] = qMin( count[i], 10 );
            }
            float div;
            for ( int i = 0; i < m_blockSize; ++i )
            {
                div = ( (float)count[i] / 20. ) + 0.5;
                data[i] *= div;
            }
            break;
        }
        case 2:
        {
            for ( unsigned int i = 0; i < fibs.size(); ++i )
            {
                Fib fib = fibs[i];
                visited.clear();

                QColor localColor = fib.customColor();
                QVector3D colVec( localColor.redF(), localColor.greenF(), localColor.blue() );

                for ( unsigned int k = 0; k < fib.length(); ++k )
                {
                    id = getID( fib[k].x(), fib[k].y(), fib[k].z() );
                    if ( !visited.contains( id) )
                    {
                        ++count[ id ];
                        data[ id ] += colVec;
                        visited.insert( id );
                    }
                }
            }

            for ( int i = 0; i < m_blockSize; ++i )
            {
                data[i] /= count[i];
            }

            break;
        }
        case 3:
        {
            int dm = m_dataset->properties( "maingl" ).get( Fn::Property::D_DATAMODE ).toInt();
            int cm = m_dataset->properties( "maingl" ).get( Fn::Property::D_COLORMAP ).toInt();
            ColormapBase cmb = ColormapFunctions::get( cm );

            float dataMin = m_dataset->getDataMins()[dm];
            float dataMax = m_dataset->getDataMaxes()[dm];
            float div = dataMax - dataMin;

            for ( unsigned int i = 0; i < fibs.size(); ++i )
            {
                Fib fib = fibs[i];
                visited.clear();

                for ( unsigned int k = 0; k < fib.length(); ++k )
                {
                    float value = ( fib.getData( dm, k ) - dataMin ) / div;

                    QColor localColor = cmb.getColor( value );
                    QVector3D colVec( localColor.redF(), localColor.greenF(), localColor.blue() );

                    id = getID( fib[k].x(), fib[k].y(), fib[k].z() );
                    if ( !visited.contains( id) )
                    {
                        ++count[ id ];
                        data[ id ] += colVec;
                        visited.insert( id );
                    }
                }
            }

            for ( int i = 0; i < m_blockSize; ++i )
            {
                data[i] /= count[i];
            }

            break;
        }

        default:
            break;
    }

    nifti_image* header = createHeader( 3 );
    Dataset3D* out = new Dataset3D( QDir( "tract color" ), data, header );
    return out;
}

DatasetFibers* Fibers::downSample()
{
    std::vector<Fib> fibs = *( m_dataset->getFibs() );
    std::vector<Fib> newFibs;

    for ( unsigned int i = 0; i < fibs.size();++i )
    {
        Fib fib = fibs[i];
        Fib newFib;

        if ( fib.length() > 2 )
        {
            for ( unsigned int k = 1; k < fib.getCountDataFields(); ++k )
            {
                newFib.addDataField();
            }

            for ( unsigned int k = 0; k < fib.length(); k += 2 )
            {
                newFib.addVert( fib[k] );

                for ( unsigned int l = 0; l < fib.getCountDataFields(); ++l )
                {
                    newFib.setData( l, newFib.length() - 1, fib.getData( l, k ) );
                }
            }

            if ( ( fib.length() ) % 2 == 1 )
            {
                newFib.addVert( fib.lastVert() );
                for ( unsigned int l = 0; l < fib.getCountDataFields(); ++l )
                {
                    newFib.setData( l, newFib.length() - 1, fib.getData( l, fib.length() - 1 ) );
                }
            }
            newFibs.push_back( newFib );
        }
    }

    DatasetFibers* out = new DatasetFibers( QDir( "new fibers" ), newFibs, m_dataset->getDataNames() );
    out->setDataMins( m_dataset->getDataMins() );
    out->setDataMaxes( m_dataset->getDataMaxes() );
    return out;
}

nifti_image* Fibers::createHeader( int dim )
{
    nifti_image* out = nifti_simple_init_nim();

    out->nx = m_nx;
    out->ny = m_ny;
    out->nz = m_nz;

    out->nvox = out->nx * out->ny * out->nz * dim;

    out->dx = m_dx;
    out->dy = m_dy;
    out->dz = m_dz;

    out->nifti_type = 1; // 1==NIFTI-1 (1 file)

    out->freq_dim = 1;
    out->phase_dim = 2;
    out->slice_dim = 3;

    out->nt = dim;
    out->nv = 4;
    out->ndim = 4;

    QMatrix4x4 qform;
    out->qform_code = 1;

    QMatrix4x4 sform;
    out->sform_code = 0;

    out->qto_xyz.m[0][0] = m_dx;
    out->qto_xyz.m[1][1] = m_dy;
    out->qto_xyz.m[2][2] = m_dz;

    out->qto_xyz.m[0][3] = m_ax + m_dx / 2;
    out->qto_xyz.m[1][3] = m_ay + m_dy / 2;
    out->qto_xyz.m[2][3] = m_az + m_dz / 2;

    {
        float dx, dy, dz;
        nifti_mat44_to_quatern( out->qto_xyz, &( out->quatern_b ), &( out->quatern_c ), &( out->quatern_d ), &( out->qoffset_x ), &( out->qoffset_y ),
                &( out->qoffset_z ), &dx, &dy, &dz, &( out->qfac ) );
    }

    out->qto_ijk = nifti_mat44_inverse( out->qto_xyz );
    out->sto_ijk = nifti_mat44_inverse( out->sto_xyz );

    out->nbyper = 4;
    out->datatype = DT_FLOAT;

    return out;
}

DatasetFibers* Fibers::unfold()
{
    std::vector<Fib> fibs = *( m_dataset->getFibs() );

    std::vector<Fib> newFibs = unfolder_sphere( fibs );

    DatasetFibers* out = new DatasetFibers( QDir( "new fibers" ), newFibs, m_dataset->getDataNames() );
    out->setDataMins( m_dataset->getDataMins() );
    out->setDataMaxes( m_dataset->getDataMaxes() );
    return out;
}

std::vector<Fib> Fibers::unfolder_sphere( std::vector<Fib>& fibs )
{
    int count = 0;
    QVector3D origin( 0, 0, 0 );

    std::vector<Fib> newFibs;

    // compute barycentre
    for( int i = 0; i < fibs.size(); ++i )
    {
        Fib fib = fibs[i];
        for( int k = 0; k < fib.length(); ++k )
        {
            origin += fib[k];
            ++count;
        }
    }
    origin /= count;

    // compute maximum radius
    float maxRadius = 0.0;
    for( int i = 0; i < fibs.size(); ++i )
    {
        Fib fib = fibs[i];
        for( int k = 0; k < fib.length(); ++k )
        {
            QVector3D vert = fib[k];
            float r = sqrt( ( vert.x() - origin.x() ) * ( vert.x() - origin.x() ) + ( vert.y() - origin.y() ) * ( vert.y() - origin.y() ) + ( vert.z() - origin.z() ) * ( vert.z() - origin.z() ) );
            if( r > maxRadius )
            {
                maxRadius = r;
            }
        }
    }

    for( int i = 0; i < fibs.size(); ++i )
    {
        Fib fib = fibs[i];
        Fib newFib;
        for( int k = 0; k < fib.length(); ++k )
        {
            QVector3D vert = fib[k];

            float r = sqrt( pow( vert.x() - origin.x(), 2 ) + pow( vert.y() - origin.y(),2 ) + pow( vert.z() - origin.z(),2 ) );
            float a = atan2( vert.y() - origin.y(), vert.x() - origin.x() );
            float d = atan2( sqrt( pow( vert.x() - origin.x(), 2 ) + pow( vert.y() - origin.y(), 2 ) ),vert.z() - origin.z() );
            d = fabs( d );
            float f = maxRadius * d / kPI;

            QVector3D newVert( f*cos(a), f*sin(a), r );
            newFib.addVert( newVert );
        }
        newFibs.push_back( newFib );
    }

    return newFibs;

}
