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

SHRendererThread2::SHRendererThread2( int id,
                                           std::vector<ColumnVector>* data,
                                           QMatrix4x4 pMatrix,
                                           QMatrix4x4 mvMatrix,
                                           PropertyGroup& props
                                                                 ) :
    m_id( id ),
    m_data( data ),
    m_pMatrix( pMatrix ),
    m_mvMatrix( mvMatrix ),
    m_mesh( 0 ),
    m_props( props )
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
    int lod = m_props.get( Fn::Property::D_LOD ).toInt();
    int numVerts = tess::n_vertices( lod );
    int numTris = tess::n_faces( lod );

    int numThreads = GLFunctions::idealThreadCount;

    std::vector<SHRendererThread*> threads;
    // create threads

    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new SHRendererThread( i, m_data, m_pMatrix, m_mvMatrix, m_props ) );
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

    std::vector<float> verts;
    // combine verts from all threads
    for ( int i = 0; i < numThreads; ++i )
    {
        verts.insert( verts.end(), threads[i]->getVerts()->begin(), threads[i]->getVerts()->end() );
        delete threads[i];
    }

   try
   {
        int numBalls = verts.size() / ( numVerts * 7 );

        m_mesh = new TriangleMesh2( numVerts * numBalls, numTris * numBalls );

        const int* faces = tess::faces( lod );

        float posX, posY, posZ;
        float newPosX, newPosY, newPosZ;
        float offsetX, offsetY, offsetZ;
        float radius;

        for ( unsigned int i = 0; i < verts.size() / 7; ++i )
        {
            posX = verts[i*7];
            posY = verts[i*7+1];
            posZ = verts[i*7+2];
            offsetX = verts[i*7+3];
            offsetY = verts[i*7+4];
            offsetZ = verts[i*7+5];
            if ( m_props.get( Fn::Property::D_HIDE_NEGATIVE_LOBES ).toBool() )
            {
                radius = qMax( 0.0f, verts[i*7+6] );
            }
            else
            {
                radius = verts[i*7+6];
            }

            float scaling = m_props.get( Fn::Property::D_SCALING ).toFloat();
            newPosX = posX * radius * scaling + offsetX;
            newPosY = posY * radius * scaling + offsetY;
            newPosZ = posZ * radius * scaling + offsetZ;

            m_mesh->addVertex( newPosX, newPosY, newPosZ );
            m_mesh->setVertexColor( i, fabs( posX), fabs( posY), fabs( posZ), 1.0f );
        }

        verts.clear();

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
        qCritical() << "bad alloc sh renderer";
    }
    m_mesh->finalize();

    emit( finished( m_mesh ) );
}
