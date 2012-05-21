/*
 * NavRenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "navrenderer.h"

#include <QtCore/QDebug>

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../data/datastore.h"

#include "gl/glfunctions.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

NavRenderer::NavRenderer( QString name ) :
    ObjectRenderer(),
    m_name( name ),
    m_ratio( 1.0 ),
    vboIds( new GLuint[ 2 ] ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_xb( 0 ),
    m_yb( 0 ),
    m_zb( 0 )
{
}

NavRenderer::~NavRenderer()
{
}

void NavRenderer::init()
{
}

void NavRenderer::initGL()
{
    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );
    static GLfloat lightPosition[ 4 ] = { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    initShader();
}

void NavRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;

    adjustRatios();
}

void NavRenderer::adjustRatios()
{
    m_ratio = static_cast< float >( m_width ) / static_cast< float >( m_height );
    glViewport( 0, 0, m_width, m_height );

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();

    if ( m_name == "axial" )
    {

        float textureRatio = m_xb / m_yb;
        float mult = m_ratio / textureRatio;

        if ( mult >= 1.0 )
        {
            pMatrix.ortho( 0, m_xb * mult, 0, m_yb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_xb, 0, m_yb * ( 1.0 / mult ), -3000, 3000 );
        }
    }
    else if ( m_name == "coronal" )
    {
        float textureRatio = m_xb / m_zb;
        float mult = m_ratio / textureRatio;
        if ( mult >= 1.0 )
        {
            pMatrix.ortho( 0, m_xb * mult, 0, m_zb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_xb, 0, m_zb * ( 1.0 / mult ), -3000, 3000 );
        }
    }
    else
    {
        float textureRatio = m_yb / m_zb;
        float mult = m_ratio / textureRatio;
        if ( mult >= 1.0 )
        {
            pMatrix.ortho( 0, m_yb * mult, 0, m_zb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_yb, 0, m_zb * ( 1.0 / mult ), -3000, 3000 );
        }
    }
    m_mvpMatrix = pMatrix;
}

void NavRenderer::leftMouseDown( int x, int y )
{
}

void NavRenderer::leftMouseDrag( int x, int y )
{
}

void NavRenderer::initShader()
{
    m_program = GLFunctions::initSliceShader();
}

void NavRenderer::initGeometry()
{
    glGenBuffers( 4, vboIds );

    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();
    m_xb = model()->data( model()->index( 0, 103 ), Qt::UserRole ).toFloat();
    m_yb = model()->data( model()->index( 0, 104 ), Qt::UserRole ).toFloat();
    m_zb = model()->data( model()->index( 0, 105 ), Qt::UserRole ).toFloat();

    float dx = model()->data( model()->index( 0, 106 ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, 107 ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, 108 ), Qt::UserRole ).toFloat();

    m_x *= dx;
    m_y *= dy;
    m_z *= dz;
    m_xb *= dx;
    m_yb *= dy;
    m_zb *= dz;


    VertexData verticesAxial[] =
    {
            { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) },
            { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) },
            { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) },
            { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) }
    };

    VertexData verticesCoronal[] =
    {
            { QVector3D( 0.0,  0.0,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, 0.0,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, m_zb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) },
            { QVector3D( 0.0,  m_zb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) }
    };

    VertexData verticesSagittal[] =
    {
            { QVector3D( 0.0,  0.0,  m_x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) },
            { QVector3D( m_yb, 0.0,  m_x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) },
            { QVector3D( m_yb, m_zb, m_x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) },
            { QVector3D( 0.0,  m_zb, m_x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) }
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesAxial, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 2
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCoronal, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 3
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesSagittal, GL_STATIC_DRAW );

}

void NavRenderer::setupTextures()
{
    GLFunctions::setupTextures( model() );
}

void NavRenderer::setShaderVars()
{
    GLFunctions::setSliceShaderVars( m_program, model() );
}

void NavRenderer::draw()
{
    //qDebug() << "nav draw";
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setupTextures();

    adjustRatios();


    // Set modelview-projection matrix
    m_program->setUniformValue( "mvp_matrix", m_mvpMatrix );

    initGeometry();

    if ( m_name == "axial" )
    {
        drawAxial();
    }
    else if ( m_name == "coronal" )
    {
        drawCoronal();
    }
    else
    {
        drawSagittal();
    }

    glDeleteBuffers( 1, &vboIds[0] );
    glDeleteBuffers( 1, &vboIds[1] );
    glDeleteBuffers( 1, &vboIds[2] );
    glDeleteBuffers( 1, &vboIds[3] );
}

void NavRenderer::drawAxial()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}

void NavRenderer::drawCoronal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}

void NavRenderer::drawSagittal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}


