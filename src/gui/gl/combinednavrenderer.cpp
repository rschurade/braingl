/*
 * NavRenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "combinednavrenderer.h"

#include <QtCore/QDebug>

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../../data/datastore.h"
#include "../../data/enums.h"

#include "../gl/glfunctions.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

CombinedNavRenderer::CombinedNavRenderer( QString name ) :
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

CombinedNavRenderer::~CombinedNavRenderer()
{
}

void CombinedNavRenderer::init()
{
}

void CombinedNavRenderer::initGL()
{
    qDebug() << "gl init " << m_name << " widget";
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

    glGenBuffers( 2, vboIds );

    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );
    static GLfloat lightPosition[ 4 ] = { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    GLFunctions::loadShaders();
}

void CombinedNavRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;

    adjustRatios();
}

void CombinedNavRenderer::adjustRatios()
{
    m_ratio = static_cast< float >( m_width ) / static_cast< float >( m_height );
    glViewport( 0, 0, m_width, m_height );

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();

    if ( m_ratio > 1.5 )
    {
        float textureRatio =  ( m_xb + m_xb + m_yb ) / m_yb;
        float mult = textureRatio / m_ratio;
        if ( m_ratio > textureRatio )
        {
            pMatrix.ortho( 0, ( m_xb + m_xb + m_yb ) / mult, 0, m_yb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_xb + m_xb + m_yb, 0, m_yb * mult , -3000, 3000 );
        }
    }
    else if ( m_ratio < 0.66 )
    {
        float textureRatio = m_yb / ( m_yb + m_zb + m_zb );
        float mult = textureRatio / m_ratio;
        //qDebug() << "ratio: " << m_ratio << " trat: " << textureRatio << " mult: " << mult;
        if ( m_ratio > textureRatio )
        {
            pMatrix.ortho( 0, m_yb / mult, 0, ( m_yb + m_zb + m_zb ), -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_yb, 0, ( m_yb + m_zb + m_zb ) * mult, -3000, 3000 );
        }


    }
    else
    {
        float mult = 1.0 / m_ratio;
        //qDebug() << "ratio: " << m_ratio << " trat: " << textureRatio << " mult: " << mult;
        if ( m_ratio > 1.0 )
        {
            pMatrix.ortho( 0, ( m_xb + m_yb ) / mult, 0, m_xb + m_yb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_xb + m_yb, 0, ( m_xb + m_yb ) * mult, -3000, 3000 );
        }
    }

     m_mvpMatrix = pMatrix;
}

void CombinedNavRenderer::leftMouseDown( int x, int y )
{
}

void CombinedNavRenderer::leftMouseDrag( int x, int y )
{
}

void CombinedNavRenderer::initGeometry()
{
    m_x = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toFloat();
    m_xb = model()->data( model()->index( 0, FNGLOBAL_MAX_SAGITTAL ), Qt::UserRole ).toFloat();
    m_yb = model()->data( model()->index( 0, FNGLOBAL_MAX_CORONAL ), Qt::UserRole ).toFloat();
    m_zb = model()->data( model()->index( 0, FNGLOBAL_MAX_AXIAL ), Qt::UserRole ).toFloat();

    float dx = model()->data( model()->index( 0, FNGLOBAL_SLICE_DX ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, FNGLOBAL_SLICE_DY ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, FNGLOBAL_SLICE_DZ ), Qt::UserRole ).toFloat();

    m_x *= dx;
    m_y *= dy;
    m_z *= dz;
    m_xb *= dx;
    m_yb *= dy;
    m_zb *= dz;

    VertexData vertices[12];

    if ( m_ratio > 1.5 )
    {
        VertexData vd0 = { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) };

        VertexData vd1 =  { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) };
        VertexData vd2 =  { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) };
        VertexData vd3 =  { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) };

        VertexData vd4 =  { QVector3D( m_xb,  0.0,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) };
        VertexData vd5 =  { QVector3D( m_xb + m_xb, 0.0,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) };
        VertexData vd6 =  { QVector3D( m_xb + m_xb, m_zb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) };
        VertexData vd7 =  { QVector3D( m_xb,  m_zb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) };

        VertexData vd8 =  { QVector3D( m_xb + m_xb,  0.0,  m_x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) };
        VertexData vd9 =  { QVector3D( m_xb + m_xb + m_yb, 0.0,  m_x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) };
        VertexData vd10 = { QVector3D( m_xb + m_xb + m_yb, m_zb, m_x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) };
        VertexData vd11 = { QVector3D( m_xb + m_xb,  m_zb, m_x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) };
        vertices[ 0 ] = vd0;
        vertices[ 1 ] = vd1;
        vertices[ 2 ] = vd2;
        vertices[ 3 ] = vd3;
        vertices[ 4 ] = vd4;
        vertices[ 5 ] = vd5;
        vertices[ 6 ] = vd6;
        vertices[ 7 ] = vd7;
        vertices[ 8 ] = vd8;
        vertices[ 9 ] = vd9;
        vertices[ 10 ] = vd10;
        vertices[ 11 ] = vd11;
    }
    else if ( m_ratio < 0.66 )
    {
        VertexData vd0 =  { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) };
        VertexData vd1 =  { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) };
        VertexData vd2 =  { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) };
        VertexData vd3 =  { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) };

        VertexData vd4 =  { QVector3D( 0.0,  m_yb,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) };
        VertexData vd5 =  { QVector3D( m_xb, m_yb,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) };
        VertexData vd6 =  { QVector3D( m_xb, m_yb + m_zb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) };
        VertexData vd7 =  { QVector3D( 0.0,  m_yb + m_zb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) };

        VertexData vd8 =  { QVector3D( 0.0,  m_yb + m_zb,  m_x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) };
        VertexData vd9 =  { QVector3D( m_yb, m_yb + m_zb,  m_x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) };
        VertexData vd10 = { QVector3D( m_yb, m_yb + m_zb + m_zb, m_x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) };
        VertexData vd11 = { QVector3D( 0.0,  m_yb + m_zb + m_zb, m_x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) };
        vertices[ 0 ] = vd0;
        vertices[ 1 ] = vd1;
        vertices[ 2 ] = vd2;
        vertices[ 3 ] = vd3;
        vertices[ 4 ] = vd4;
        vertices[ 5 ] = vd5;
        vertices[ 6 ] = vd6;
        vertices[ 7 ] = vd7;
        vertices[ 8 ] = vd8;
        vertices[ 9 ] = vd9;
        vertices[ 10 ] = vd10;
        vertices[ 11 ] = vd11;
    }
    else
    {
        VertexData vd0 =  { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) };
        VertexData vd1 =  { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) };
        VertexData vd2 =  { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) };
        VertexData vd3 =  { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) };

        VertexData vd4 =  { QVector3D( 0.0,  m_yb,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) };
        VertexData vd5 =  { QVector3D( m_xb, m_yb,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) };
        VertexData vd6 =  { QVector3D( m_xb, m_zb + m_yb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) };
        VertexData vd7 =  { QVector3D( 0.0,  m_zb + m_yb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) };

        VertexData vd8 =  { QVector3D( m_xb,  m_yb,  m_x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) };
        VertexData vd9 =  { QVector3D( m_yb + m_xb, m_yb,  m_x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) };
        VertexData vd10 = { QVector3D( m_yb + m_xb, m_zb + m_yb, m_x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) };
        VertexData vd11 = { QVector3D( m_xb,  m_zb + m_yb, m_x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) };
        vertices[ 0 ] = vd0;
        vertices[ 1 ] = vd1;
        vertices[ 2 ] = vd2;
        vertices[ 3 ] = vd3;
        vertices[ 4 ] = vd4;
        vertices[ 5 ] = vd5;
        vertices[ 6 ] = vd6;
        vertices[ 7 ] = vd7;
        vertices[ 8 ] = vd8;
        vertices[ 9 ] = vd9;
        vertices[ 10 ] = vd10;
        vertices[ 11 ] = vd11;
    }

    GLushort indices[] = { 0, 1, 2, 0, 2, 3,
                           4, 5, 6, 4, 6, 7,
                           8, 9, 10, 8, 10, 11};

    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 18 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(VertexData), vertices, GL_STATIC_DRAW );
}

void CombinedNavRenderer::setupTextures()
{
    GLFunctions::setupTextures( model() );
}

void CombinedNavRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "slice", model() );
}

void CombinedNavRenderer::draw()
{
    //qDebug() << "combined draw";
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setupTextures();

    adjustRatios();

    GLFunctions::getShader( "slice" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "slice" )->setUniformValue( "mvp_matrix", m_mvpMatrix );

    initGeometry();

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0 );
}
