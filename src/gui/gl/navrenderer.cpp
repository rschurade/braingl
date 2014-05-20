/*
 * NavRenderer.cpp
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */
#include "navrenderer.h"

#include "../gl/glfunctions.h"

#include <QDebug>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

NavRenderer::NavRenderer( QString name ) :
    m_name( name ),
    m_ratio( 1.0 ),
    vboIds( new GLuint[ 4 ] ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_nx( 0 ),
    m_ny( 0 ),
    m_nz( 0 ),
    m_dx( 0 ),
    m_dy( 0 ),
    m_dz( 0 ),
    m_xOld( -1 ),
    m_yOld( -1 ),
    m_zOld( -1 ),
    m_xoff( 0 ),
    m_yoff( 0 ),
    m_zoom( 1 ),
    m_middleDownX( -1 ),
    m_middleDownY( -1 ),
    m_moveX( 0 ),
    m_moveY( 0 ),
    m_moveXOld( 0 ),
    m_moveYOld( 0 )
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
    qDebug() << "gl init " << m_name << " widget";

    glewExperimental = true;
    GLenum errorCode = glewInit();
    if ( GLEW_OK != errorCode )
    {
        qDebug() << "Problem: glewInit failed, something is seriously wrong.";
        qDebug() << glewGetErrorString( errorCode );
        exit( false );
    }
    else
    {
        //qDebug() << "OpenGL initialized.";
    }

    glGenBuffers( 4, vboIds );

    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    //glShadeModel( GL_SMOOTH );    // XXX not in Core
    //glEnable( GL_LIGHTING );    // XXX not in CoreProfile; use shader
    //glEnable( GL_LIGHT0 );    // XXX not in CoreProfile; use shader
    glEnable( GL_MULTISAMPLE );
    //static GLfloat lightPosition[ 4 ] = { 0.5, 5.0, -3000.0, 1.0 };
    //glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );   // XXX not in Core

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    GLushort crosshairIndices[] = { 0, 1, 2, 3 };

    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    // Transfer index data to VBO 3
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 3 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), crosshairIndices, GL_STATIC_DRAW );
}

void NavRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;

    adjustRatios();
}

void NavRenderer::leftMouseDrag( int x, int y )
{
    leftMouseDown( x, y );
}

void NavRenderer::setupTextures()
{
    GLFunctions::setupTextures();
}

void NavRenderer::setShaderVars()
{
    QGLShaderProgram* program = GLFunctions::getShader( "slice" );
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0 );
    GLFunctions::setTextureUniforms( program, "maingl" );
}

void NavRenderer::mouseWheel( int step )
{
    m_zoom += step;
    m_zoom = qMax( 1, m_zoom );
}

void NavRenderer::middleMouseDown( int x, int y )
{
    m_moveXOld = m_moveX;
    m_moveYOld = m_moveY;
    m_middleDownX = x;
    m_middleDownY = y;
}

void NavRenderer::middleMouseDrag( int x, int y )
{
    m_moveX = m_moveXOld - ( m_middleDownX - x );
    m_moveY = m_moveYOld + m_middleDownY - y;
}
