/*
 * fiberstipplingrenderer.cpp
 *
 *  Created on: May 6, 2014
 *      Author: schurade
 */

#include "fiberstipplingrenderer.h"

#include "../gl/glfunctions.h"

#include "../../algos/tree.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/vptr.h"

#include "../../data/datasets/datasettree.h"

#include <QDebug>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

FiberStipplingRenderer::FiberStipplingRenderer( QString name ) :
    ObjectRenderer(),
    m_name( name ),
    vboIds( new GLuint[ 2 ] ),
    m_width( 0 ),
    m_height( 0 ),
    m_zoom( 1 ),
    m_moveX( 0 ),
    m_moveY( 0 ),
    m_moveXOld( 0 ),
    m_moveYOld( 0 ),
    m_middleDownX( 0 ),
    m_middleDownY( 0 )
{
}

FiberStipplingRenderer::~FiberStipplingRenderer()
{
}

void FiberStipplingRenderer::init()
{
}

void FiberStipplingRenderer::initGL()
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

    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glEnable( GL_MULTISAMPLE );
}

void FiberStipplingRenderer::draw()
{
    glViewport( 0, 0, m_width, m_height );

//    QColor color = Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_NAV2, 0 ) ).value<QColor>();
//    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Reset projection
   QMatrix4x4 pMatrix;
   pMatrix.setToIdentity();

   if ( m_iso1Verts.size() != 0 )
   {
       drawIso1( pMatrix );
   }
}

void FiberStipplingRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;
}

void FiberStipplingRenderer::leftMouseDown( int x, int y )
{
}

void FiberStipplingRenderer::leftMouseDrag( int x, int y )
{
    leftMouseDown( x, y );
}

void FiberStipplingRenderer::setShaderVars()
{
}

void FiberStipplingRenderer::mouseWheel( int step )
{
    m_zoom += step;
    m_zoom = qMax( 1, m_zoom );
}

void FiberStipplingRenderer::middleMouseDown( int x, int y )
{
    m_moveXOld = m_moveX;
    m_moveYOld = m_moveY;
    m_middleDownX = x;
    m_middleDownY = y;
}

void FiberStipplingRenderer::middleMouseDrag( int x, int y )
{
    m_moveX = m_moveXOld - ( m_middleDownX - x ) * 10;
    m_moveY = m_moveYOld + m_middleDownY - y;
}

void FiberStipplingRenderer::setIso1( std::vector<float> verts )
{
    qDebug() << "renderer: set iso 1";
    qDebug() << verts.size();
    m_iso1Verts = verts;

    // Transfer vertex data to VBO 3
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof( float ), verts.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    m_colors.resize( ( verts.size() / 3 ) * 4, 0.5f );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, m_colors.size() * sizeof( float ), m_colors.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void FiberStipplingRenderer::drawIso1( QMatrix4x4 mvpMatrix )
{
    qDebug() << "renderer: draw iso 1";

    GLFunctions::getShader( "line" )->bind();
    GLFunctions::getShader( "line" )->setUniformValue( "mvp_matrix", mvpMatrix );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    // Draw cube geometry using indices from VBO 0
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = GLFunctions::getShader( "line" )->attributeLocation( "a_position" );
    GLFunctions::getShader( "line" )->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    // Draw cube geometry using indices from VBO 1
    int colorLocation = GLFunctions::getShader( "line" )->attributeLocation( "a_color" );
    GLFunctions::getShader( "line" )->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0 );


    // Draw cube geometry using indices from VBO 0
    glDrawArrays( GL_LINES, 0 , m_iso1Verts.size() / 2 );
}
