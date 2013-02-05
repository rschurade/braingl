/*
 * fibers.cpp
 *
 *  Created on: Jan 24, 2013
 *      Author: schurade
 */
#include "fibers.h"

#include "../data/datasets/datasetfibers.h"
#include "../data/datasets/datasetscalar.h"

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
    return new DatasetFibers( m_dataset->getRoiModel(), fibs + unmergedFibs );
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
    QVector< QVector< float > > fibs = m_dataset->getFibs();
    int x, y, z;
    for ( int i = 0; i < fibs.size(); ++i )
    {
        for( int k = 0; k < fibs[i].size() / 3; ++k )
        {
            x = fibs[i].at( k * 3     );
            y = fibs[i].at( k * 3 + 1 );
            z = fibs[i].at( k * 3 + 2 );
            ++data[ getID( x, y, z ) ];
        }
    }

    int dims[8] = { 3, 160, 200, 160, 1, 1, 1 };
    nifti_image* header = nifti_make_new_nim( dims, NIFTI_TYPE_FLOAT32, 1 );
    DatasetScalar* out = new DatasetScalar( "tract density", data, header );
    return out;
}
