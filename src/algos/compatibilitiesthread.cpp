/*
 * compatibilitiesthread.cpp
 *
 *  Created on: Dec 11, 2013
 *      Author: boettgerj
 */

#include "compatibilitiesthread.h"

#include "../gui/gl/glfunctions.h"

CompatibilitiesThread::CompatibilitiesThread( int id, float c_thr, QList<Edge*> edges, Compatibilities* compatibilities ) :
        m_id( id ),
        m_c_thr( c_thr ),
        m_edges( edges ),
        m_compatibilities( compatibilities )
{
}

CompatibilitiesThread::~CompatibilitiesThread()
{

}

void CompatibilitiesThread::run()
{
    int numThreads = GLFunctions::idealThreadCount;

    for ( int i = m_id; i < m_edges.length(); i += numThreads )
    {
        if ( ( i % 1000 ) == 0 )
        {
            qDebug() << "calculating compatibilites: " << i;
            emit( progress() );
        }
        for ( int j = 0; j < m_edges.length(); j++ )
        {
            if ( i == j )
            {
                m_compatibilities->addComp( i, j, 1 );
            }
            else
            {
                Edge* ei = m_edges.at( i );
                Edge* ej = m_edges.at( j );
                //calculate compatibility btw. edge i and j
                //angle
                double angle_comp;
                if ( !ei->flip( ej ) )
                {
                    angle_comp = QVector3D::dotProduct( ei->fn - ei->tn, ej->tn - ej->fn );
                }
                else
                {
                    angle_comp = QVector3D::dotProduct( ei->fn - ei->tn, ej->fn - ej->tn );
                }
                angle_comp /= ei->length() * ej->length();
                //length
                double lavg = ( ei->length() + ej->length() ) / 2.0;
                double l_comp = 2 / ( ( lavg / qMin( ei->length(), ej->length() ) ) + ( qMax( ei->length(), ej->length() ) / lavg ) );
                //position
                QVector3D mi = ( ei->fn + ei->tn ) / 2;
                QVector3D mj = ( ej->fn + ej->tn ) / 2;
                double p_comp = lavg / ( lavg + ( mi - mj ).length() );
                //visibility
                if ( angle_comp * l_comp * p_comp > 0.9 )
                {
                    double vis_comp = qMin( vis_c( ei, ej ), vis_c( ej, ei ) );
                    //comps[i + edges.size() * j] = angle_comp * l_comp * p_comp * vis_comp;
                    double prod = angle_comp * l_comp * p_comp * vis_comp;
                    if ( prod > m_c_thr )
                    {
                        m_compatibilities->addComp( i, j, prod );
                    }
                }
                else
                {
                    //comps[i + edges.size() * j] = angle_comp * l_comp * p_comp;
                    double prod = angle_comp * l_comp * p_comp;
                    if ( prod > m_c_thr )
                    {
                        m_compatibilities->addComp( i, j, prod );
                    }
                }

            }
        }
    }
    emit( finished() );
}

double CompatibilitiesThread::vis_c( Edge* ep, Edge* eq )
{
    QVector3D i0 = proj( ep->fn, ep->tn, eq->fn );
    QVector3D i1 = proj( ep->fn, ep->tn, eq->tn );
    QVector3D im = ( i0 + i1 ) / 2;
    QVector3D pm = ( ep->fn + ep->tn ) / 2;

    return qMax( 1 - 2 * ( pm - im ).length() / ( i0 - i1 ).length(), 0.0 );
}

QVector3D CompatibilitiesThread::proj( QVector3D a, QVector3D b, QVector3D p )
{
    QVector3D ba = b - a;
    QVector3D pa = p - a;
    return a + ba * QVector3D::dotProduct( ba, pa ) / ba.lengthSquared();
}
