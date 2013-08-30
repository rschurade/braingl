/*
 * shrendererthread.cpp
 *
 * Created on: 27.12.2012
 * @author Ralph Schurade
 */
#include "shrendererthread2.h"

#include "shrendererthread.h"

#include "glfunctions.h"

#include "../../algos/fmath.h"
#include "../../data/mesh/tesselation.h"
#include "../../data/mesh/trianglemesh2.h"
#include "../../data/models.h"

#include <math.h>
#include <limits>

SHRendererThread2::SHRendererThread2( int id, QVector<ColumnVector>* data,
                                                                int nx, int ny, int nz,
                                                                float dx, float dy, float dz,
                                                                int xi, int yi, int zi,
                                                                int lod,
                                                                int order,
                                                                int orient,
                                                                bool scaling,
                                                                float scaleFactor,
                                                                QMatrix4x4 pMatrix,
                                                                QMatrix4x4 mvMatrix
                                                                 ) :
    m_id( id ),
    m_data( data ),
    m_nx( nx ),
    m_ny( ny ),
    m_nz( nz ),
    m_dx( dx ),
    m_dy( dy ),
    m_dz( dz ),
    m_xi( xi ),
    m_yi( yi ),
    m_zi( zi ),
    m_lod( lod ),
    m_order( order ),
    m_orient( orient ),
    m_scaling( scaling ),
    m_scaleFactor( scaleFactor ),
    m_pMatrix( pMatrix ),
    m_mvMatrix( mvMatrix ),
    m_mesh( 0 )
{
}

SHRendererThread2::~SHRendererThread2()
{
}

TriangleMesh2* SHRendererThread2::getMesh()
{
    return m_mesh;
}

void SHRendererThread2::run()
{
    //qDebug() << "SH Renderer: using lod " << lod;

    int numVerts = tess::n_vertices( m_lod );
    int numTris = tess::n_faces( m_lod );

    int numThreads = GLFunctions::idealThreadCount;

    QVector<SHRendererThread*> threads;
    // create threads

    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new SHRendererThread( i, m_data, m_nx, m_ny, m_nz, m_dx, m_dy, m_dz, m_xi, m_yi, m_zi, m_lod,
                                                   m_order, m_orient, m_scaling, m_pMatrix, m_mvMatrix ) );
    }

    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->wait();
    }

    QVector<float> verts;
    // combine verts from all threads
    for ( int i = 0; i < numThreads; ++i )
    {
        verts += *( threads[i]->getVerts() );
        delete threads[i];
    }

   try
   {
        int numBalls = verts.size() / ( numVerts * 7 );

        m_mesh = new TriangleMesh2( numVerts * numBalls, numTris * numBalls );

        const int* faces = tess::faces( m_lod );

        float posX, posY, posZ;
        float newPosX, newPosY, newPosZ;
        float offsetX, offsetY, offsetZ;
        float radius;

        for ( int i = 0; i < verts.size() / 7; ++i )
        {
            posX = verts[i*7];
            posY = verts[i*7+1];
            posZ = verts[i*7+2];
            offsetX = verts[i*7+3];
            offsetY = verts[i*7+4];
            offsetZ = verts[i*7+5];
            radius = verts[i*7+6];

            newPosX = posX * radius * m_scaleFactor + offsetX;
            newPosY = posY * radius * m_scaleFactor + offsetY;
            newPosZ = posZ * radius * m_scaleFactor + offsetZ;

            m_mesh->addVertex( newPosX, newPosY, newPosZ );
            m_mesh->setVertexColor( i, fabs( posX), fabs( posY), fabs( posZ), 1.0f );
        }

        verts.clear();
        verts.squeeze();

        for ( int currentBall = 0; currentBall < numBalls; ++currentBall )
        {
            for ( int i = 0; i < numTris; ++i )
            {
                m_mesh->addTriangle( faces[i*3] + numVerts * currentBall, faces[i*3+2] + numVerts * currentBall, faces[i*3+1] + numVerts * currentBall );
            }
        }

    }
    catch (std::bad_alloc& ba)
    {
        qDebug() << "bad alloc sh renderer";
    }
    m_mesh->finalize();

    emit( finished( m_mesh ) );
}
