/*
 * bundlethread.cpp
 *
 * Created on: 04.11.2013
 * @author Ralph Schurade
 */
#include "bundlethread.h"
#include "fmath.h"
#include "kdtree.h"

#include "../gui/gl/glfunctions.h"

#include "time.h"
#include "math.h"

BundleThread::BundleThread( int id,
                               std::vector<Fib>* fibs,
                               KdTree* kdTree,
                               std::vector<float>* kdVerts,
                               std::vector<unsigned int>* revInd,
                               int numPoints ) :
    m_id( id ),
    m_fibs( fibs ),
    m_kdTree( kdTree ),
    m_kdVerts( kdVerts ),
    m_revInd( revInd ),
    m_radius( 5.0f ),
    m_numPoints( numPoints ),
    m_iterations( 10 )
{
    m_boxMin.resize( 3 );
    m_boxMax.resize( 3 );
}

BundleThread::~BundleThread()
{
    m_forces.clear();
}

std::vector< QVector3D > BundleThread::getForces()
{
    return m_forces;
}

void BundleThread::run()
{
    // for each point run algo
    calculateForces();

    emit( finished() );
}

void BundleThread::calculateForces()
{
    int numThreads = GLFunctions::idealThreadCount;
    for ( unsigned int i = m_id; i < m_fibs->size(); i += numThreads )
    {
        Fib fib = m_fibs->at( i );
        for( unsigned int k = 1; k < fib.length() - 1; ++k )
        {
            QVector3D start = fib.firstVert();
            QVector3D end = fib.lastVert();

            QVector3D point = fib[k];
            m_boxMin[0] = point.x() - m_radius;
            m_boxMax[0] = point.x() + m_radius;
            m_boxMin[1] = point.y() - m_radius;
            m_boxMax[1] = point.y() + m_radius;
            m_boxMin[2] = point.z() - m_radius;
            m_boxMax[2] = point.z() + m_radius;
            std::vector<QVector3D>result;
            std::vector<unsigned int>revInds;

            boxTest( result, revInds, 0, m_numPoints - 1, 0 );
            QVector3D center( 0, 0, 0 );

            int countIn = 0;
            for( unsigned int l = 0; l < result.size(); ++l )
            {
                int curFib = revInds[l];
                Fib fib1 = m_fibs->at( curFib );
                QVector3D start1 = fib1.firstVert();
                QVector3D end1 = fib1.lastVert();

                if ( ( start - start1 ).length() < 20 ||
                     ( start - end1 ).length()   < 20 ||
                     ( end - start1 ).length()   < 20 ||
                     ( end - end1 ).length()     < 20 )
                {
                    center += result[l];
                    ++countIn;
                }
            }
            center /= countIn;
            QVector3D force = ( center - point );
            m_forces.push_back( force );
        }
    }
}

void BundleThread::boxTest( std::vector<QVector3D>& result, std::vector<unsigned int>& revInds, unsigned int left, unsigned int right, int axis )
{
    // abort condition
    if ( left > right || right == std::numeric_limits<unsigned int>::max() )
    {
        return;
    }

    int root = left + ( ( right - left ) / 2 );
    int axis1 = ( axis + 1 ) % 3;
    int pointIndex = m_kdTree->m_tree[root] * 3;

    if ( m_kdVerts->at( pointIndex + axis ) < m_boxMin[axis] )
    {
        boxTest( result, revInds, root + 1, right, axis1 );
    }
    else if ( m_kdVerts->at( pointIndex + axis ) > m_boxMax[axis] )
    {
        boxTest( result, revInds, left, root - 1, axis1 );
    }
    else
    {
        int axis2 = ( axis + 2 ) % 3;
        if ( m_kdVerts->at( pointIndex + axis1 ) <= m_boxMax[axis1] &&
             m_kdVerts->at( pointIndex + axis1 ) >= m_boxMin[axis1] &&
             m_kdVerts->at( pointIndex + axis2 ) <= m_boxMax[axis2] &&
             m_kdVerts->at( pointIndex + axis2 ) >= m_boxMin[axis2] )
        {
            result.push_back( QVector3D( m_kdVerts->at( pointIndex ), m_kdVerts->at( pointIndex+1 ), m_kdVerts->at( pointIndex+2 ) ) );
            revInds.push_back( m_revInd->at( m_kdTree->m_tree[root] ) );
        }
        boxTest( result, revInds, left, root - 1, axis1 );
        boxTest( result, revInds, root + 1, right, axis1 );
    }
}
