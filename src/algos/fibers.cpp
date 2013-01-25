/*
 * fibers.cpp
 *
 *  Created on: Jan 24, 2013
 *      Author: schurade
 */
#include <limits>

#include "../data/datasets/datasetfibers.h"

#include "fibers.h"

Fibers::Fibers( DatasetFibers* ds ) :
    m_dataset( ds )
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
    return new DatasetFibers( fibs + unmergedFibs );
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
