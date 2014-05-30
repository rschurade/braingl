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

#include <QSet>
#include <QVector3D>

#include <limits>
#include <math.h>

Fibers::Fibers( DatasetFibers* ds ) :
    m_dataset( ds ),
    m_nx( 1 ),
    m_ny( 1 ),
    m_nz( 1 ),
    m_dx( 1.0 ),
    m_dy( 1.0 ),
    m_dz( 1.0 ),
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
            //if ( i % 1000 == 0 ) qDebug() << i << fibs.size() << mergedFibs.size() << unmergedFibs.size();
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
    m_dx = res;
    m_dy = res;
    m_dz = res;
    m_nx = 160;
    m_ny = 200;
    m_nz = 160;

    m_nx = ( ( m_nx * m_dx ) / res ) + 1;
    m_ny = ( ( m_ny * m_dy ) / res ) + 1;
    m_nz = ( ( m_nz * m_dz ) / res ) + 1;


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

    int dims[8] = { 3, m_nx, m_ny, m_nz, 1, 1, 1 };
    nifti_image* header = nifti_make_new_nim( dims, NIFTI_TYPE_FLOAT32, 1 );
    header->dx = m_dx;
    header->dy = m_dy;
    header->dz = m_dz;
    DatasetScalar* out = new DatasetScalar( QDir( "tract density" ), data, header );
    return out;
}

Dataset3D* Fibers::tractColor()
{
    m_dx = Models::getGlobal( Fn::Property::G_TRACT_TEX_RESOLUTION ).toFloat();
    m_dy = m_dx;
    m_dz = m_dx;
    m_nx = 160 / m_dx;
    m_ny = 200 / m_dy;
    m_nz = 160 / m_dz;
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
        default:
            break;
    }

    int dims[8] = { 3, m_nx, m_ny, m_nz, 3, 1, 1 };
    nifti_image* header = nifti_make_new_nim( dims, NIFTI_TYPE_FLOAT32, 1 );
    header->dx = m_dx;
    header->dy = m_dy;
    header->dz = m_dz;
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
