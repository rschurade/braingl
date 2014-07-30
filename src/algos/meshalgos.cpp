/*
 * meshalgos.cpp
 *
 *  Created on: Sep 3, 2013
 *      Author: schurade
 */

#include "meshalgos.h"

#include "loopsubdivision.h"

#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasetfmri.h"
#include "../data/datasets/datasetmesh.h"
#include "../data/datasets/datasetmeshtimeseries.h"
#include "../data/mesh/trianglemesh2.h"

#include "../data/models.h"

#include <QQueue>

QList<Dataset*> MeshAlgos::loopSubdivision( Dataset* ds )
{
    LoopSubdivision loop( dynamic_cast<DatasetMesh*>( ds )->getMesh() );
    DatasetMesh* dsm = new DatasetMesh( loop.getMesh(), QDir("loop subdivision") );
    QList<Dataset*> l;
    l.push_back( dsm );
    return l;
}

QList<Dataset*> MeshAlgos::meshTimeSeries( Dataset* ds, TriangleMesh2* mesh )
{
    DatasetMeshTimeSeries* dataset = new DatasetMeshTimeSeries( QDir("mesh time series"), Fn::DatasetType::MESH_TIME_SERIES );

    dataset->addMesh( mesh );

    if ( dynamic_cast<DatasetFMRI*>( ds ) )
    {
        DatasetFMRI* fmri = dynamic_cast<DatasetFMRI*>( ds );

        int nx = fmri->properties( "maingl" ).get( Fn::Property::D_NX ).toInt();
        int ny = fmri->properties( "maingl" ).get( Fn::Property::D_NY ).toInt();
        int nz = fmri->properties( "maingl" ).get( Fn::Property::D_NZ ).toInt();
        int dim = fmri->properties( "maingl" ).get( Fn::Property::D_DIM ).toInt();
        std::vector<float>* data = fmri->getData();

        int numVerts = mesh->numVerts();

        for ( int k = 0; k < dim; ++k )
        {
            std::vector<float>timeStep;
            int offset = nx * ny * nz * k;
            for ( int i = 0; i < numVerts; ++i )
            {
                timeStep.push_back( data->at( fmri->getIdFromPos( mesh->getVertex( i ) ) + offset ) );
            }
            dataset->addData( timeStep );
        }
    }
    dataset->setProperties();

    QList<Dataset*> l;
    l.push_back( dataset );
    return l;
}

QList<Dataset*> MeshAlgos::biggestComponent( Dataset* ds )
{
    DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( ds );

    TriangleMesh2* mesh = dsm->getMesh();
    qDebug() << "in:  num verts:" << mesh->numVerts() << "num tris:" << mesh->numTris();

    int numTris = mesh->numTris();

    QHash<int, int>todo;
    for ( int i = 0; i < numTris; ++i )
    {
        todo.insert( i, i );
    }
    QQueue<int>queue;
    std::vector<std::vector<int> >components;
    int sumTris = 0;
    do
    {
        std::vector<int>component;
        int item = todo.take( todo.begin().value() );
        queue.enqueue( item );
        while( !queue.empty() )
        {
            int currentTri = queue.dequeue();
            component.push_back( currentTri );
            std::vector<unsigned int>tri = mesh->getTriangle( currentTri );

            for( unsigned int i = 0; i < tri.size(); ++i )
            {
                int vert = tri[i];
                std::vector<unsigned int>star = mesh->getStar( vert );
                for ( unsigned int k = 0; k < star.size(); ++k )
                {
                    int nextTri = star[k];
                    if ( todo.contains( nextTri ) )
                    {
                        todo.remove( nextTri );
                        queue.enqueue( nextTri );
                    }
                }
            }
        }
        components.push_back( component );
        sumTris += component.size();
        //qDebug() << "new component of size:" << component.size();
    }
    while( sumTris < numTris );

    QList<Dataset*> l;

    for ( unsigned int i = 0; i < components.size(); ++i )
    {
        if ( (int)components[i].size() < Models::getGlobal( Fn::Property::G_MIN_COMPONENT_SIZE ).toInt() )
        {
            continue;
        }

        TriangleMesh2* newMesh = pruneMesh( mesh, components[i] );
        qDebug() << "out: num verts:" << newMesh->numVerts() << "num tris:" << newMesh->numTris();
        DatasetMesh* newDSM = new DatasetMesh( newMesh, "mesh component " + QString::number( i ) );
        l.push_back( newDSM );
    }




   return l;
}

TriangleMesh2* MeshAlgos::pruneMesh( TriangleMesh2* mesh, std::vector<int>component )
{
    QMap<int, int>newVertIds;

    for( unsigned int k = 0; k < component.size(); ++k )
    {
        std::vector<unsigned int>tri = mesh->getTriangle( component[k] );
        for( unsigned int l = 0; l < tri.size(); ++l )
        {
            if ( !newVertIds.contains( tri[l] ) )
            {
                newVertIds.insert( tri[l], 0 );
            }
        }
    }

    TriangleMesh2* newMesh = new TriangleMesh2( newVertIds.size(), component.size() );

    QMap<int, int>::iterator mapIterator = newVertIds.begin();
    int vertInsertId = 0;
    while ( mapIterator != newVertIds.end() )
    {
        QVector3D vert = mesh->getVertex( mapIterator.key() );
        newVertIds[mapIterator.key()] = vertInsertId++;
        newMesh->addVertex( vert.x(), vert.y(), vert.z() );
        ++mapIterator;
    }


    for( unsigned int k = 0; k < component.size(); ++k )
    {
        std::vector<unsigned int>tri = mesh->getTriangle( component[k] );
        newMesh->addTriangle( newVertIds[tri[0]], newVertIds[tri[1]], newVertIds[tri[2]] );
    }

    newMesh->finalize();
    return newMesh;
}

QList<Dataset*> MeshAlgos::decimate( Dataset* ds )
{
    DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( ds );
    TriangleMesh2* mesh = dsm->getMesh();
    QList<Dataset*> l;

    qDebug() << "in:  num verts:" << mesh->numVerts() << "num tris:" << mesh->numTris();

    float epsilon = Models::getGlobal( Fn::Property::G_DECIMATE_EPSILON).toFloat();

    QMap<QString, std::vector<int> >cells;
    std::vector<QString>keys( mesh->numVerts() );

    // put verts into cells of diameter epsilon
    int x,y,z;
    for ( unsigned int i = 0; i < mesh->numVerts(); ++i )
    {
        QVector3D vert = mesh->getVertex( i );
        x = vert.x() / epsilon;
        y = vert.y() / epsilon;
        z = vert.z() / epsilon;
        QString key = QString( x ) + "_" + QString( y ) + "_" + QString( z );
        keys[i] = key;

        if ( cells.contains( key ) )
        {
            cells[key].push_back( i );
        }
        else
        {
            std::vector<int> entry;
            entry.push_back( i );
            cells.insert( key, entry );
        }
    }

    // for each cell compute representative vertex

    QMap<QString, std::vector<int> >::iterator mapIterator = cells.begin();

    QMap<QString, Point>newVerts;

    int newPointId = 0;

    while( mapIterator != cells.end() )
    {
        std::vector<int>verts = mapIterator.value();

        QVector3D newVert( 0, 0, 0 );
        for ( unsigned int i = 0; i < verts.size(); ++i )
        {
            newVert += mesh->getVertex( verts[i] );
        }
        newVert /= verts.size();
        Point p;
        p.newID = newPointId++;
        p.pos = newVert;
        newVerts.insert( mapIterator.key(), p );

        ++mapIterator;
    }

    std::vector<Triangle>newTriangles;

    // remove degenerative triangles
    for ( unsigned int i = 0; i < mesh->numTris(); ++i )
    {
        Triangle t = mesh->getTriangle2( i );
        QString key0 = keys[t.v0];
        QString key1 = keys[t.v1];
        QString key2 = keys[t.v2];
        Triangle newT;
        newT.v0 = newVerts[key0].newID;
        newT.v1 = newVerts[key1].newID;
        newT.v2 = newVerts[key2].newID;

        if ( ( newT.v0 != newT.v1 ) &&  ( newT.v0 != newT.v2 ) &&  ( newT.v1 != newT.v2 ) )
        {
            newTriangles.push_back( newT );
        }
    }

    TriangleMesh2* newMesh = new TriangleMesh2( newVerts.size(), newTriangles.size() );

    QMap<QString, Point>::iterator vertIterator = newVerts.begin();

    while( vertIterator != newVerts.end() )
    {
        newMesh->setVertex( vertIterator.value().newID, vertIterator.value().pos );
        ++vertIterator;
    }
    for ( unsigned int i = 0; i < newTriangles.size(); ++i )
    {
        newMesh->setTriangle( i, newTriangles[i] );
    }
    newMesh->finalize();
    qDebug() << "out: num verts:" << newMesh->numVerts() << "num tris:" << newMesh->numTris();
    DatasetMesh* newDSM = new DatasetMesh( newMesh, QString("mesh decimate") );
    l.push_back( newDSM );

    return l;
}
