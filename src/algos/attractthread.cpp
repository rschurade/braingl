/*
 * attractthread.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: boettgerj
 */

#include "attractthread.h"

#include "../gui/gl/glfunctions.h"

#include "qmath.h"

AttractThread::AttractThread( int id, float bell, QList<Edge*> edges, Compatibilities* compatibilities ) :
        m_id( id ),
        m_bell( bell ),
        m_edges( edges ),
        m_compatibilities( compatibilities )
{

}

AttractThread::~AttractThread()
{
    // TODO Auto-generated destructor stub
}

void AttractThread::run()
{
    int numThreads = GLFunctions::idealThreadCount;

    for ( int ie = m_id; ie < m_edges.length(); ie += numThreads )
    {
        Edge* e = m_edges.at( ie );
        //for every point...
        for ( int i = 1; i < e->points.length() - 1; i++ )
        {
            QVector3D p = e->points.at( i );
            double fsum = 0;
            QVector3D f( 0, 0, 0 );
            //for all attracting points...
            for ( unsigned int ef = 0; ef < m_compatibilities->idxs->at( ie )->size(); ef++ )
            {
                QVector3D pe;
                int idx = m_compatibilities->idxs->at( ie )->at( ef );
                if ( e->flip( m_edges.at( idx ) ) )
                {
                    pe = m_edges.at( idx )->points.at( i );
                }
                else
                {
                    pe = m_edges.at( idx )->points.at( ( m_edges.at( idx )->points.length() - 1 ) - i );
                }
                float de = ( pe - p ).length();
                double weight = qExp( -( de * de ) / ( 2 * m_bell * m_bell ) );
                fsum += weight;
                f += weight * pe;
            }

            f /= fsum;
            QVector3D force = ( f - p );
            e->forces.replace( i, force );
        }
    }
}
