/*
 * BinghamRenderer.cpp
 *
 * Created on: 03.07.2012
 * @author Ralph Schurade
 */
#include "binghamrenderer.h"
#include "binghamrendererthread.h"

#include "glfunctions.h"

#include "../../data/datasets/datasetsh.h"
#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../algos/fmath.h"
#include "../../algos/qball.h"

#include "../../data/mesh/tesselation.h"
#include "../../data/properties/propertygroup.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>
#include <QVector3D>
#include <QMatrix4x4>

#include <limits>

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
    m_offset( 0 ),
    m_lodAdjust( 0 ),
    m_minMaxScaling( true),
    m_order( 4 ),
    m_render1( true ),
    m_render2( false ),
    m_render3( false )
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

void BinghamRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, PropertyGroup* props )
{
    int renderMode = GLFunctions::renderMode;
    if ( !( renderMode == 4 || renderMode == 5) ) // we are drawing opaque objects
    {
        // obviously not opaque
        return;
    }

    setRenderParams( props );

    if ( m_orient == 0 )
    {
        return;
    }

    m_pMatrix = p_matrix;
    m_mvMatrix = mv_matrix;


    initGeometry();

    QGLShaderProgram* program = GLFunctions::getShader( "qball" );
    program->bind();

    program->setUniformValue( "u_alpha", 1.0f );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", GLFunctions::getScreenSize().x(), GLFunctions::getScreenSize().y() );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );
    program->setUniformValue( "u_hideNegativeLobes", m_minMaxScaling );
    program->setUniformValue( "u_scaling", m_scaling );

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
    QGLShaderProgram* program = GLFunctions::getShader( "qball" );

    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float) * 3;
    int offsetLocation = program->attributeLocation( "a_offset" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float) * 3;
    int radiusLocation = program->attributeLocation( "a_radius" );
    program->enableAttributeArray( radiusLocation );
    glVertexAttribPointer( radiusLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );
}

void BinghamRenderer::initGeometry()
{
    float dx = model()->data( model()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = model()->data( model()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = model()->data( model()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    int xi = model()->data( model()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat() * ( dx / m_dx );
    int yi = model()->data( model()->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat() * ( dy / m_dy );
    int zi = model()->data( model()->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat() * ( dz / m_dz );

    xi = qMax( 0, qMin( xi, m_nx - 1) );
    yi = qMax( 0, qMin( yi, m_ny - 1) );
    zi = qMax( 0, qMin( zi, m_nz - 1) );

    float zoom = model()->data( model()->index( (int)Fn::Global::ZOOM, 0 ) ).toFloat();
    float moveX = model()->data( model()->index( (int)Fn::Global::MOVEX, 0 ) ).toFloat();
    float moveY = model()->data( model()->index( (int)Fn::Global::MOVEY, 0 ) ).toFloat();


    int renderPeaks = (int)m_render1 * 1 + (int)m_render2 * 2 + (int)m_render3 * 4;

    int lod = m_lodAdjust;

    QString s = createSettingsString( {xi, yi, zi, m_orient, m_minMaxScaling, renderPeaks, lod, zoom, moveX, moveY, m_offset } );

    if ( s == m_previousSettings || m_orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    //qDebug() << "Bingham Renderer: using lod " << lod;

    int numVerts = tess::n_vertices( lod );
    int numTris = tess::n_faces( lod );

    int numThreads = GLFunctions::idealThreadCount;

    QVector<BinghamRendererThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new BinghamRendererThread( i, m_data, m_nx, m_ny, m_nz, m_dx, m_dy, m_dz, xi + m_offset, yi + m_offset, zi + m_offset, lod,
                                                      m_order, m_orient, m_minMaxScaling, renderPeaks,
                                                      m_pMatrix, m_mvMatrix ) );
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

    int numBalls = verts.size() / ( numVerts * 7 );
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

void BinghamRenderer::setRenderParams( PropertyGroup* props )
{
    int slice = (int)props->get( Fn::Property::RENDER_AXIAL ).toBool() +
                    (int)props->get( Fn::Property::RENDER_CORONAL ).toBool() * 2 +
                    (int)props->get( Fn::Property::RENDER_SAGITTAL ).toBool() * 4;

    m_scaling = props->get( Fn::Property::SCALING ).toFloat();
    m_orient = slice;
    m_offset = props->get( Fn::Property::OFFSET ).toFloat();
    m_lodAdjust = props->get( Fn::Property::LOD ).toInt();
    m_minMaxScaling = props->get( Fn::Property::MINMAX_SCALING ).toBool();
    m_order = props->get( Fn::Property::ORDER ).toInt();
    m_render1 = props->get( Fn::Property::RENDER_FIRST ).toBool();
    m_render2 = props->get( Fn::Property::RENDER_SECOND ).toBool();
    m_render3 = props->get( Fn::Property::RENDER_THIRD ).toBool();
}
