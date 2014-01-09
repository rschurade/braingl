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
    QVector< QVector< float > > fibs = m_dataset->getFibs();
    QVector< QVector< float > > mergedFibs;
    QVector< QVector< float > > unmergedFibs;

    while ( merged )
    {
        qDebug() << "iteration:" << iteration;
        ++iteration;
        qDebug() << fibs.size() << "fibers before merging";

        merged = false;
        QVector<bool>processed( fibs.size(), false );

        for ( int i = 0; i < fibs.size(); ++i )
        {
            if ( i % 1000 == 0 ) qDebug() << i << fibs.size() << mergedFibs.size() << unmergedFibs.size();
            currentMerged = false;
            QVector< float > currentFib = fibs[ i ];
            for( int k = i + 1; k < fibs.size(); ++k )
            {
                if ( currentFib.size() == fibs[k].size() )
                {
                    float dist = getFiberDist( currentFib, fibs[k] );
                    if ( dist < maxDistance )
                    {
                        mergedFibs.push_back( mergeFibs( currentFib, fibs[k]) );
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
    return new DatasetFibers( QDir( "new fibers" ), fibs + unmergedFibs );
}

QVector<float> Fibers::mergeFibs( QVector< float >& lhs, QVector< float >& rhs )
{
    QVector<float> out( lhs.size() );
    for ( int i = 0; i < lhs.size(); ++i )
    {
        out[i] = ( lhs[i] + rhs[i] ) / 2;
    }
    return out;
}

float Fibers::getFiberDist( QVector< float >& lhs, QVector< float >& rhs )
{
    float distSum = 0;
    float minSum;
    float lhsx;
    float lhsy;
    float lhsz;
    for ( int i = 0; i < lhs.size(); i += 3 )
    {
        minSum = std::numeric_limits<float>::max();
        lhsx = lhs.at( i );
        lhsy = lhs.at( i + 1 );
        lhsz = lhs.at( i + 2 );
        for( int k = 0; k < rhs.size(); k += 3 )
        {
            minSum = qMin( minSum, getDist( lhsx, lhsy, lhsz, rhs.at( i ), rhs.at( i + 1 ), rhs.at( i + 2 ) ) );
        }
        distSum += minSum;
    }
    distSum /= lhs.size();

    return distSum;
}

float Fibers::getDist( float x1, float y1, float z1, float x2, float y2, float z2 )
{
    float x = x1 - x2;
    float y = y1 - y2;
    float z = z1 - z2;
    return sqrt( x*x + y*y + z*z );
}

DatasetScalar* Fibers::tractDensity()
{
    m_nx = 160;
    m_ny = 200;
    m_nz = 160;
    m_dx = 1.0;
    m_dy = 1.0;
    m_dz = 1.0;
    m_blockSize = m_nx * m_ny * m_nz;
    QVector<float> data( m_blockSize, 0 );
    QVector< QVector< float > > fibs = m_dataset->getSelectedFibs();
    int x, y, z;
    QVector3D p3;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        QVector<float> fibdata( m_blockSize, 0 );

        for( int k = 0; k < ( fibs[i].size() / 3 ) - 1; ++k )
        {
            QVector3D p1( fibs[i].at( k * 3     ) + 0.5, fibs[i].at( k * 3 + 1 ) + 0.5, fibs[i].at( k * 3 + 2 ) + 0.5 );
            QVector3D p2( fibs[i].at( k * 3 + 3 ) + 0.5, fibs[i].at( k * 3 + 4 ) + 0.5, fibs[i].at( k * 3 + 5 ) + 0.5 );
            p2 = p2 - p1;
            p2.normalize();

            for ( int l = 0; l < 10; ++l )
            {
                p3 = p1 + ( ( (float)l / 10 ) * p2 ) ;
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

    int dims[8] = { 3, 160, 200, 160, 1, 1, 1 };
    nifti_image* header = nifti_make_new_nim( dims, NIFTI_TYPE_FLOAT32, 1 );
    DatasetScalar* out = new DatasetScalar( QDir( "tract density" ), data, header );
    return out;
}

Dataset3D* Fibers::tractColor()
{
    m_nx = 320;
    m_ny = 400;
    m_nz = 320;
    m_dx = 0.5;
    m_dy = 0.5;
    m_dz = 0.5;
    m_blockSize = m_nx * m_ny * m_nz;
    QVector<QVector3D> data( m_blockSize );
    QVector<int> count( m_blockSize, 0 );
    QVector< QVector< float > > fibs = m_dataset->getSelectedFibs();
    float x, y, z;
    QSet<int> visited;
    int id = 0;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        QVector<float> fib = fibs[i];
        visited.clear();

        QVector3D localColor( fabs( fib[0] - fib[3] ), fabs( fib[1] - fib[4] ), fabs( fib[2] - fib[5] ) );
        localColor.normalize();
        x = fib.at( 0 );
        y = fib.at( 1 );
        z = fib.at( 2 );
        id = getID( x, y, z );
        if ( !visited.contains( id) )
        {
            ++count[ id ];
            data[ id ] += localColor;
            visited.insert( id );
        }

        for ( int k = 1; k < fib.size() / 3 - 1; ++k )
        {
            QVector3D localColor( fabs( fib[k*3-3] - fib[k*3+3] ), fabs( fib[k*3-2] - fib[k*3+4] ), fabs( fib[k*3-1] - fib[k*3+5] ) );
            localColor.normalize();

            x = fib.at( k * 3 );
            y = fib.at( k * 3 + 1 );
            z = fib.at( k * 3 + 2 );
            id = getID( x, y, z );
            if ( !visited.contains( id) )
            {
                ++count[ id ];
                data[ id ] += localColor;
                visited.insert( id );
            }
        }

        int numFloats = fib.size();
        QVector3D localColor2( fabs( fib[numFloats-6] - fib[numFloats-3] ), fabs( fib[numFloats-5] - fib[numFloats-2] ), fabs( fib[numFloats-4] - fib[numFloats-1] ) );
        localColor2.normalize();

        x = fib.at( numFloats - 3 );
        y = fib.at( numFloats - 2 );
        z = fib.at( numFloats - 1 );
        id = getID( x, y, z );
        if ( !visited.contains( id) )
        {
            ++count[ id ];
            data[ id ] += localColor2;
            visited.insert( id );
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

    int dims[8] = { 3, m_nx, m_ny, m_nz, 3, 1, 1 };
    nifti_image* header = nifti_make_new_nim( dims, NIFTI_TYPE_FLOAT32, 1 );
    header->dx = m_dx;
    header->dy = m_dy;
    header->dz = m_dz;
    Dataset3D* out = new Dataset3D( QDir( "tract color" ), data, header );
    return out;
}

DatasetFibers* Fibers::resample()
{
    QVector< QVector< float > > fibs = m_dataset->getFibs();
    QVector< QVector< float > > newFibs;

    for ( int i = 0; i < fibs.size();++i )
    {
        QVector<float> fib = fibs[i];
        QVector<float> newFib;

        if ( fib.size() > 6 )
        {
            for ( int k = 0; k < fib.size() / 3; k += 2 )
            {
                newFib.push_back( fib[k*3] );
                newFib.push_back( fib[k*3+1] );
                newFib.push_back( fib[k*3+2] );
            }

            if ( ( fib.size() / 3 ) % 2 == 1 )
            {
                newFib.push_back( fib[fib.size()-3] );
                newFib.push_back( fib[fib.size()-2] );
                newFib.push_back( fib[fib.size()-1] );
            }
            newFibs.push_back( newFib );
        }
    }


    return new DatasetFibers( QDir( "new fibers" ), newFibs );
}
