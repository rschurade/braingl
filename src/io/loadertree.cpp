/*
 * loadertree.cpp
 *
 *  Created on: 18.08.2014
 *      Author: Ralph
 */

#include "loadertree.h"

#include "loaderfreesurfer.h"

#include "../data/datasets/datasettree.h"

#include "../data/mesh/trianglemesh2.h"

#include <QDebug>
#include <QTextStream>

LoaderTree::LoaderTree( QString fileName ) :
    m_fileName( fileName ),
    m_dataset( 0 ),
    m_meshoffsetX( 0 ),
    m_meshoffsetY( 0 ),
    m_meshoffsetZ( 0 )
{
}

LoaderTree::~LoaderTree()
{
}

bool LoaderTree::load()
{
    m_dataset = new DatasetTree( m_fileName );

    qDebug() << "loading tree: " << m_fileName;

    QFile treeFile( m_fileName );
    if ( !treeFile.open( QIODevice::ReadOnly ) )
    {
        qCritical( "tree file unreadable" );
        return false;
    }
    QTextStream ts( &treeFile );
    QString nl;

    QString dims;
    QString meshoffset;
    std::vector<QString>voxels;
    std::vector<QString>nodes;
    QFileInfo fi( m_fileName );
    while ( !ts.atEnd() )
    {
        nl = ts.readLine();
        if ( nl.startsWith( "#imagesize" ) )
        {
            dims = ts.readLine();
        }

        if ( nl.startsWith( "#voxels" ) )
        {
            nl = ts.readLine();
            while( nl != "#endvoxels" )
            {
                voxels.push_back( nl );
                nl = ts.readLine();
            }
        }

        if ( nl.startsWith( "#nodes" ) )
        {
            nl = ts.readLine();
            while( nl != "#endnodes" )
            {
                nodes.push_back( nl );
                nl = ts.readLine();
            }
        }

        if ( nl.startsWith( "#meshoffset" ) )
        {
            meshoffset = ts.readLine();
            QStringList mol = meshoffset.split( ' ' );
            if ( mol.size() == 3 )
            {
                m_meshoffsetX = mol[0].toFloat();
                m_meshoffsetY = mol[1].toFloat();
                m_meshoffsetZ = mol[2].toFloat();
            }
        }


        if ( nl.startsWith( "#surface" ) )
        {
            nl = ts.readLine();
            while( nl != "#endsurface" )
            {
                if ( !loadSurfaceMesh( fi.path() + QDir::separator() + nl ) )
                {
                    qCritical() << "unable to load: " << fi.path() + QDir::separator() + nl;
                }
                nl = ts.readLine();
            }
        }

        if ( nl.startsWith( "#projection" ) )
        {
            nl = ts.readLine();
            if ( !loadProjection( fi.path() + QDir::separator() + nl ) )
            {
                qCritical() << "unable to load: " << fi.path() + QDir::separator() + nl;
            }
        }
    }
    m_dataset->setProperties();
    m_dataset->importTree( dims, voxels, nodes );

    return true;
}


bool LoaderTree::loadSurfaceMesh( QString fileName )
{
    LoaderFreesurfer lf;
    if ( !lf.loadASC( fileName ) )
    {
        return false;
    }

    std::vector<float>* points = lf.getPoints();
    std::vector<int> triangles = lf.getTriangles();
    int numPoints = points->size() / 3;
    int numTriangles = triangles.size() / 3;

    TriangleMesh2* mesh = new TriangleMesh2( numPoints, numTriangles );
    for ( int i = 0; i < numPoints; ++i )
    {
        mesh->addVertex( points->at( i * 3 ) + m_meshoffsetX, points->at( i * 3 + 1 ) + m_meshoffsetY, points->at( i * 3 + 2 ) + m_meshoffsetZ );
    }
    for ( int i = 0; i < numTriangles; ++i )
    {
        //TODO: Check orientation change (0,2,1)...
        mesh->addTriangle( triangles[i * 3], triangles[i * 3 + 2], triangles[i * 3 + 1] );
    }
    mesh->finalize();
    QFileInfo fi( fileName );
    m_dataset->addMesh( mesh, fi.fileName() );

    return true;
}

bool LoaderTree::loadProjection( QString fileName )
{
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return false;
    }
    QTextStream ts( &file );

    QString nl = ts.readLine();
    QStringList sl = nl.split( ' ' );
    std::vector<int>projection;

    for ( int i = 0; i < sl.size(); ++i )
    {
        projection.push_back( sl[i].toInt() );
    }
    m_dataset->setProjection( projection );
    return true;
}
