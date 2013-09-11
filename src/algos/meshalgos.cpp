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
