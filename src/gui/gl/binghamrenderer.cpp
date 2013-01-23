/*
 * BinghamRenderer.cpp
 *
 *  Created on: 03.07.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include <limits>

#include <QtOpenGL/QGLShaderProgram>
#include <QtCore/QDebug>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../../data/datasets/datasetsh.h"
#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../algos/fmath.h"
#include "../../algos/qball.h"

#include "../../data/mesh/tesselation.h"

#include "../../thirdparty/newmat10/newmat.h"

#include "glfunctions.h"
#include "binghamrendererthread.h"
#include "binghamrenderer.h"

BinghamRenderer::BinghamRenderer( QVector<QVector<float> >* data, int m_nx, int m_ny, int m_nz, float m_dx, float m_dy, float m_dz ) :
    ObjectRenderer(),
    m_tris1( 0 ),
    vboIds( new GLuint[ 6 ] ),
    m_data( data ),
    m_nx( m_nx ),
    m_ny( m_ny ),
    m_nz( m_nz ),
    m_dx( m_dx ),
    m_dy( m_dy ),
    m_dz( m_dz ),
    m_scaling( 1.0 ),
    m_orient( 0 ),
    m_offset( 0.0 ),
    m_lodAdjust( 0 ),
    m_minMaxScaling( false ),
    m_order( 4 )
{
}

BinghamRenderer::~BinghamRenderer()
{
    glDeleteBuffers(1, &( vboIds[ 0 ] ) );
    glDeleteBuffers(1, &( vboIds[ 1 ] ) );
}

void BinghamRenderer::init()
{
    glGenBuffers( 2, vboIds );
}

void BinghamRenderer::draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert )
{
    if ( m_orient == 0 )
    {
        return;
    }

    GLFunctions::getShader( "qball" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "qball" )->setUniformValue( "mvp_matrix", mvp_matrix );
    GLFunctions::getShader( "qball" )->setUniformValue( "mv_matrixInvert", mv_matrixInvert );
    GLFunctions::getShader( "qball" )->setUniformValue( "u_hideNegativeLobes", m_minMaxScaling );


    initGeometry();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();
    glDrawElements( GL_TRIANGLES, m_tris1, GL_UNSIGNED_INT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void BinghamRenderer::setupTextures()
{
}

void BinghamRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "qball", model() );
}

void BinghamRenderer::initGeometry()
{
    int xi = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toInt();


    calcBounds( m_nx, m_ny, m_nz, m_dx, m_dy, m_dz, m_orient );

    int lowerX = m_visibleArea[0];
    int lowerY = m_visibleArea[2];
    int lowerZ = m_visibleArea[4];
    int upperX = m_visibleArea[1];
    int upperY = m_visibleArea[3];
    int upperZ = m_visibleArea[5];

    int _lod = m_lodAdjust - 2;
    int lod = qMin( 5, qMax( 0, getMaxLod( m_orient, lowerX, upperX, lowerY, upperY, lowerZ, upperZ ) + _lod ) );

    QString s = createSettingsString( xi, yi, zi, m_orient, lowerX, upperX, lowerY, upperY, lowerZ, upperZ, m_minMaxScaling, 0, lod);
    if ( s == m_previousSettings || m_orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    //qDebug() << "Bingham Renderer: using lod " << lod;

    int numVerts = tess::n_vertices( lod );
    int numTris = tess::n_faces( lod );

    int numThreads = QThread::idealThreadCount();

    QVector<BinghamRendererThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new BinghamRendererThread( m_data, m_nx, m_ny, m_nz, m_dx, m_dy, m_dz, xi, yi, zi, m_visibleArea, lod, m_order, m_orient, m_minMaxScaling, i ) );
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
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }

    int numBalls = verts.size() / ( numVerts * 10 );
    m_tris1 = numTris * numBalls * 3;

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW );
    verts.clear();

    const int* faces = tess::faces( lod );
    std::vector<int>indexes;
    indexes.reserve( m_tris1 );

    for ( int currentBall = 0; currentBall < numBalls; ++currentBall )
    {
        for ( int i = 0; i < numTris; ++i )
        {
            indexes.push_back( faces[i*3] + numVerts * currentBall );
            indexes.push_back( faces[i*3+1] + numVerts * currentBall );
            indexes.push_back( faces[i*3+2] + numVerts * currentBall );
        }
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );
    indexes.clear();
}

void BinghamRenderer::setRenderParams( float scaling, int orient, float offset, int lodAdjust, bool minMaxScaling, int order )
{
    m_scaling = scaling;
    m_orient = orient;
    m_offset = offset;
    m_lodAdjust = lodAdjust;
    m_minMaxScaling = minMaxScaling;
    m_order = order;
}