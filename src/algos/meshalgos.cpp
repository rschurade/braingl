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

        int nx = fmri->properties( "maingl" )->get( Fn::Property::D_NX ).toInt();
        int ny = fmri->properties( "maingl" )->get( Fn::Property::D_NY ).toInt();
        int nz = fmri->properties( "maingl" )->get( Fn::Property::D_NZ ).toInt();
        int dim = fmri->properties( "maingl" )->get( Fn::Property::D_DIM ).toInt();
        QVector<float>* data = fmri->getData();

        int numVerts = mesh->numVerts();

        for ( int k = 0; k < dim; ++k )
        {
            QVector<float>timeStep;
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

    int numTris = mesh->numTris();

    QHash<int, int>todo;
    for ( int i = 0; i < numTris; ++i )
    {
        todo.insert( i, i );
    }
    QQueue<int>queue;
    QVector<QVector<int> >components;
    int sumTris = 0;
    do
    {
        QVector<int>component;
        int item = todo.take( todo.begin().value() );
        queue.enqueue( item );
        while( !queue.empty() )
        {
            int currentTri = queue.dequeue();
            component.push_back( currentTri );
            QVector<int>tri = mesh->getTriangle( currentTri );

            for( int i = 0; i < tri.size(); ++i )
            {
                int vert = tri[i];
                QVector<int>star = mesh->getStar( vert );
                for ( int k = 0; k < star.size(); ++k )
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
        qDebug() << "new component of size:" << component.size();
    }
    while( sumTris < numTris );

    QList<Dataset*> l;

    for ( int i = 0; i < components.size(); ++i )
    {
        if ( components[i].size() < 1000 )
        {
            continue;
        }

        QVector<int>component = components[i];
        QMap<int, int>newVertIds;

        for( int k = 0; k < component.size(); ++k )
        {
            QVector<int>tri = mesh->getTriangle( component[k] );
            for( int l = 0; l < tri.size(); ++l )
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


        for( int k = 0; k < component.size(); ++k )
        {
            QVector<int>tri = mesh->getTriangle( component[k] );
            newMesh->addTriangle( newVertIds[tri[0]], newVertIds[tri[1]], newVertIds[tri[2]] );
        }

        newMesh->finalize();

        DatasetMesh* newDSM = new DatasetMesh( newMesh, "mesh component " + QString::number( i ) );
        l.push_back( newDSM );
    }




   return l;
}
