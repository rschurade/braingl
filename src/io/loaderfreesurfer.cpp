/*
 * loaderfreesurfer.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: boettgerj
 */

#include "loaderfreesurfer.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

LoaderFreesurfer::LoaderFreesurfer()
{
    m_points = new std::vector<float>();
}

LoaderFreesurfer::~LoaderFreesurfer()
{
    m_points->clear();
}

std::vector<float>* LoaderFreesurfer::getPoints()
{
    return m_points;
}

std::vector<int> LoaderFreesurfer::getTriangles()
{
    return m_triangles;
}

bool LoaderFreesurfer::loadASC( QString fn )
{
    QFile n( fn );
    if ( !n.open( QIODevice::ReadOnly ) )
    {
        qCritical() << "nodes unreadable" << fn;
        return false;
    }
    QTextStream ns( &n );

    QString nl;
    ns.readLine();
    nl = ns.readLine();
    int numPoints = nl.split( " " ).at( 0 ).toInt();
    int numTriangles = nl.split( " " ).at( 1 ).toInt();

    //POINTS
    for ( int i = 0; i < numPoints; i++ )
    {
        nl = ns.readLine();
        QStringList vals = nl.split( " ", QString::SkipEmptyParts );
        m_points->push_back( vals.at( 0 ).toFloat() );
        m_points->push_back( vals.at( 1 ).toFloat() );
        m_points->push_back( vals.at( 2 ).toFloat() );
    }

    //TRIANGLES
    for ( int i = 0; i < numTriangles; i++ )
    {
        nl = ns.readLine();
        QStringList vals = nl.split( " ", QString::SkipEmptyParts );
        m_triangles.push_back( vals.at( 0 ).toInt() );
        m_triangles.push_back( vals.at( 1 ).toInt() );
        m_triangles.push_back( vals.at( 2 ).toInt() );
    }
    return true;
}
