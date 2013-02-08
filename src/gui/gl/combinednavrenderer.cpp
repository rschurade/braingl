/*
 * NavRenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph Schurade
 */
#include "combinednavrenderer.h"
#include "../gl/glfunctions.h"

#include "../../data/enums.h"

#include <QtCore/QDebug>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>
#include <QtOpenGL/QGLShaderProgram>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

CombinedNavRenderer::CombinedNavRenderer( QAbstractItemModel* dataModel, QString name ) :
    ObjectRenderer(),
    m_dataModel( dataModel ),
    m_name( name ),
    m_ratio( 1.0 ),
    vboIds( new GLuint[ 4 ] ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_nx( 0 ),
    m_ny( 0 ),
    m_nz( 0 )
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

    glGenBuffers( 4, vboIds );

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

    float xb = m_nx * m_dx;
    float yb = m_ny * m_dy;
    float zb = m_nz * m_dz;

    if ( m_ratio > 1.5 )
    {
        float textureRatio =  ( xb + xb + yb ) / yb;
        float mult = textureRatio / m_ratio;
        if ( m_ratio > textureRatio )
        {
            pMatrix.ortho( 0, ( xb + xb + yb ) / mult, 0, yb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, xb + xb + yb, 0, yb * mult , -3000, 3000 );
        }
    }
    else if ( m_ratio < 0.66 )
    {
        float textureRatio = yb / ( yb + zb + zb );
        float mult = textureRatio / m_ratio;
        //qDebug() << "ratio: " << m_ratio << " trat: " << textureRatio << " mult: " << mult;
        if ( m_ratio > textureRatio )
        {
            pMatrix.ortho( 0, yb / mult, 0, ( yb + zb + zb ), -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, yb, 0, ( yb + zb + zb ) * mult, -3000, 3000 );
        }


    }
    else
    {
        float mult = 1.0 / m_ratio;
        //qDebug() << "ratio: " << m_ratio << " trat: " << textureRatio << " mult: " << mult;
        if ( m_ratio > 1.0 )
        {
            pMatrix.ortho( 0, ( xb + yb ) / mult, 0, xb + yb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, xb + yb, 0, ( xb + yb ) * mult, -3000, 3000 );
        }
    }

     m_mvpMatrix = pMatrix;
}

void CombinedNavRenderer::leftMouseDown( int x, int y )
{
    float xf = static_cast<float>( x );
    float yf = static_cast<float>( m_height - y );

    QVector4D test;
    test.setX( ( 2 * xf ) / m_width - 1 );
    test.setY( ( 2 * yf ) / m_height - 1 );
    test.setZ( 1 );
    test.setW( 1 );

    int xout;
    int yout;

    QVector4D out = m_mvpMatrix.inverted() * test;
    QModelIndex mi;
    if ( m_ratio > 1.5 )
    {
        xout = out.x() / m_dx;
        if ( ( (float)out.x()  / m_dx ) < m_nx )
        {
            xout = qMax( 0, qMin( xout, static_cast<int>( m_nx - 1.0 ) ) );
            yout = out.y() / m_dy;
            yout = qMax( 0, qMin( yout, static_cast<int>( m_ny - 1.0 ) ) );
            mi = model()->index( (int)Fn::Global::SAGITTAL_CORONAL, 0 );
        }
        else if ( ( (float)out.x() / m_dx ) < m_nx * 2  )
        {
            xout = xout - m_nx;
            xout = qMax( 0, qMin( xout, static_cast<int>( m_nx - 1.0 ) ) );
            yout = out.y() / m_dz;
            yout = qMax( 0, qMin( yout, static_cast<int>( m_nz - 1.0 ) ) );
            mi = model()->index( (int)Fn::Global::SAGITTAL_AXIAL, 0 );
        }
        else
        {
            xout = xout - ( 2 * m_nx );
            xout = qMax( 0, qMin( m_ny - xout - 1, static_cast<int>( m_ny - 1.0 ) ) );
            yout = out.y() / m_dz;
            yout = qMax( 0, qMin( yout, static_cast<int>( m_nz - 1.0 ) ) );
            mi = model()->index( (int)Fn::Global::CORONAL_AXIAL, 0 );
        }
    }
    else if ( m_ratio < 0.66 )
    {
        yout = out.y() / m_dz;
        if ( ( (float)out.y() / m_dz ) < m_nz )
        {
            xout = out.x() / m_dy;
            xout = qMax( 0, qMin( m_ny - xout - 1, static_cast<int>( m_ny - 1.0 ) ) );
            yout = qMax( 0, qMin( yout, static_cast<int>( m_nz - 1.0 ) ) );
            mi = model()->index( (int)Fn::Global::CORONAL_AXIAL, 0 );
        }
        else if ( ( (float)out.y() / m_dz ) < m_nz * 2  )
        {
            xout = out.x() / m_dx;
            xout = qMax( 0, qMin( xout, static_cast<int>( m_nx - 1.0 ) ) );
            yout = yout - m_nz;
            yout = qMax( 0, qMin( yout, static_cast<int>( m_nz - 1.0 ) ) );
            mi = model()->index( (int)Fn::Global::SAGITTAL_AXIAL, 0 );
        }
        else
        {
            xout = out.x() / m_dx;
            xout = qMax( 0, qMin( xout, static_cast<int>( m_nx - 1.0 ) ) );
            yout = yout - ( 2 * m_nz );
            yout = qMax( 0, qMin( yout, static_cast<int>( m_ny - 1.0 ) ) );
            mi = model()->index( (int)Fn::Global::SAGITTAL_CORONAL, 0 );
        }
    }
    else
    {
        xout = out.x() / m_dx;
        yout = out.y() / m_dy;
        if ( ( (float)out.x()  / m_dx ) < m_nx )
        {
            if ( ( (float)out.y() / m_dy ) < m_nz )
            {
                xout = qMax( 0, qMin( xout, static_cast<int>( m_nx - 1.0 ) ) );
                yout = qMax( 0, qMin( yout, static_cast<int>( m_ny - 1.0 ) ) );
                mi = model()->index( (int)Fn::Global::SAGITTAL_CORONAL, 0 );
            }
            else
            {
                xout = qMax( 0, qMin( xout, static_cast<int>( m_nx - 1.0 ) ) );
                yout = yout - m_ny;
                yout = qMax( 0, qMin( yout, static_cast<int>( m_nz - 1.0 ) ) );
                mi = model()->index( (int)Fn::Global::SAGITTAL_AXIAL, 0 );
            }
        }
        else
        {
            xout = xout - m_nx;
            xout = qMax( 0, qMin( m_ny - xout- 1, static_cast<int>( m_ny - 1.0 ) ) );
            yout = yout - m_ny;
            yout = qMax( 0, qMin( yout, static_cast<int>( m_nz - 1.0 ) ) );
            mi = model()->index( (int)Fn::Global::CORONAL_AXIAL, 0 );
        }
    }
    QPoint p( xout, yout );

    if ( mi.isValid() )
    {
        model()->setData( mi, p );
    }
}

void CombinedNavRenderer::leftMouseDrag( int x, int y )
{
    leftMouseDown( x, y );
}

void CombinedNavRenderer::initGeometry()
{
    m_x = model()->data( model()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toInt();
    m_y = model()->data( model()->index( (int)Fn::Global::CORONAL, 0 ) ).toInt();
    m_z = model()->data( model()->index( (int)Fn::Global::AXIAL, 0 ) ).toInt();
    m_nx = model()->data( model()->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toInt();
    m_ny = model()->data( model()->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toInt();
    m_nz = model()->data( model()->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toInt();

    m_dx = model()->data( model()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    m_dy = model()->data( model()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    m_dz = model()->data( model()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    float x = m_x * m_dx + m_dx / 2.0;
    float y = m_y * m_dy + m_dy / 2.0;
    float z = m_z * m_dz + m_dz / 2.0;
    float xb = m_nx * m_dx;
    float yb = m_ny * m_dy;
    float zb = m_nz * m_dz;

    VertexData vertices[12];
    VertexData verticesCrosshair[12];

    float xOffset = 0;
    float yOffset = 0;

    if ( m_ratio > 1.5 )
    {
        // axial
        vertices[0] =  { QVector3D( xOffset,      yOffset,      z ), QVector3D( 0.0, 0.0, z/zb ) };
        vertices[1] =  { QVector3D( xOffset + xb, yOffset,      z ), QVector3D( 1.0, 0.0, z/zb ) };
        vertices[2] =  { QVector3D( xOffset + xb, yOffset + yb, z ), QVector3D( 1.0, 1.0, z/zb ) };
        vertices[3] =  { QVector3D( xOffset,      yOffset + yb, z ), QVector3D( 0.0, 1.0, z/zb ) };

        verticesCrosshair[0] = { QVector3D( xOffset,      yOffset + y,  zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[1] = { QVector3D( xOffset + xb, yOffset + y,  zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[2] = { QVector3D( xOffset + x,  yOffset,      zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[3] = { QVector3D( xOffset + x,  yOffset + yb, zb ), QVector3D( 0.0, 0.0, 0.0 ) };

        xOffset = xb;
        //coronal
        vertices[4] =  { QVector3D( xOffset,      yOffset,      y ), QVector3D( 0.0, y/yb, 0.0 ) };
        vertices[5] =  { QVector3D( xOffset + xb, yOffset,      y ), QVector3D( 1.0, y/yb, 0.0 ) };
        vertices[6] =  { QVector3D( xOffset + xb, yOffset + zb, y ), QVector3D( 1.0, y/yb, 1.0 ) };
        vertices[7] =  { QVector3D( xOffset,      yOffset + zb, y ), QVector3D( 0.0, y/yb, 1.0 ) };

        verticesCrosshair[4] = { QVector3D( xOffset,      yOffset + z,  yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[5] = { QVector3D( xOffset + xb, yOffset + z,  yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[6] = { QVector3D( xOffset + x,  yOffset,      yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[7] = { QVector3D( xOffset + x,  yOffset + zb, yb ), QVector3D( 0.0, 0.0, 0.0 ) };

        xOffset += xb;
        //sagittal
        vertices[8] =  { QVector3D( xOffset,      yOffset,      x ), QVector3D( x/xb, 1.0, 0.0 ) };
        vertices[9] =  { QVector3D( xOffset + yb, yOffset,      x ), QVector3D( x/xb, 0.0, 0.0 ) };
        vertices[10] = { QVector3D( xOffset + yb, yOffset + zb, x ), QVector3D( x/xb, 0.0, 1.0 ) };
        vertices[11] = { QVector3D( xOffset,      yOffset + zb, x ), QVector3D( x/xb, 1.0, 1.0 ) };

        verticesCrosshair[8] =  { QVector3D( xOffset,          yOffset + z,  xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[9] =  { QVector3D( xOffset + yb,     yOffset + z,  xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[10] = { QVector3D( xOffset + yb - y, yOffset,      xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[11] = { QVector3D( xOffset + yb - y, yOffset + zb, xb ), QVector3D( 0.0, 0.0, 0.0 ) };



    }
    else if ( m_ratio < 0.66 )
    {
        //sagittal
        vertices[8] =  { QVector3D( xOffset,      yOffset,      x ), QVector3D( x/xb, 1.0, 0.0 ) };
        vertices[9] =  { QVector3D( xOffset + yb, yOffset,      x ), QVector3D( x/xb, 0.0, 0.0 ) };
        vertices[10] = { QVector3D( xOffset + yb, yOffset + zb, x ), QVector3D( x/xb, 0.0, 1.0 ) };
        vertices[11] = { QVector3D( xOffset,      yOffset + zb, x ), QVector3D( x/xb, 1.0, 1.0 ) };

        verticesCrosshair[8] =  { QVector3D( xOffset,          yOffset + z,  xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[9] =  { QVector3D( xOffset + yb,     yOffset + z,  xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[10] = { QVector3D( xOffset + yb - y, yOffset,      xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[11] = { QVector3D( xOffset + yb - y, yOffset + zb, xb ), QVector3D( 0.0, 0.0, 0.0 ) };

        yOffset = zb;
        //coronal
        vertices[4] =  { QVector3D( xOffset,      yOffset,      y ), QVector3D( 0.0, y/yb, 0.0 ) };
        vertices[5] =  { QVector3D( xOffset + xb, yOffset,      y ), QVector3D( 1.0, y/yb, 0.0 ) };
        vertices[6] =  { QVector3D( xOffset + xb, yOffset + zb, y ), QVector3D( 1.0, y/yb, 1.0 ) };
        vertices[7] =  { QVector3D( xOffset,      yOffset + zb, y ), QVector3D( 0.0, y/yb, 1.0 ) };

        verticesCrosshair[4] = { QVector3D( xOffset,      yOffset + z,  yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[5] = { QVector3D( xOffset + xb, yOffset + z,  yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[6] = { QVector3D( xOffset + x,  yOffset,      yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[7] = { QVector3D( xOffset + x,  yOffset + zb, yb ), QVector3D( 0.0, 0.0, 0.0 ) };

        yOffset += zb;
        // axial
        vertices[0] =  { QVector3D( xOffset,      yOffset,      z ), QVector3D( 0.0, 0.0, z/zb ) };
        vertices[1] =  { QVector3D( xOffset + xb, yOffset,      z ), QVector3D( 1.0, 0.0, z/zb ) };
        vertices[2] =  { QVector3D( xOffset + xb, yOffset + yb, z ), QVector3D( 1.0, 1.0, z/zb ) };
        vertices[3] =  { QVector3D( xOffset,      yOffset + yb, z ), QVector3D( 0.0, 1.0, z/zb ) };

        verticesCrosshair[0] = { QVector3D( xOffset,      yOffset + y,  zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[1] = { QVector3D( xOffset + xb, yOffset + y,  zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[2] = { QVector3D( xOffset + x,  yOffset,      zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[3] = { QVector3D( xOffset + x,  yOffset + yb, zb ), QVector3D( 0.0, 0.0, 0.0 ) };
    }
    else
    {
        // axial
        vertices[0] =  { QVector3D( xOffset,      yOffset,      z ), QVector3D( 0.0, 0.0, z/zb ) };
        vertices[1] =  { QVector3D( xOffset + xb, yOffset,      z ), QVector3D( 1.0, 0.0, z/zb ) };
        vertices[2] =  { QVector3D( xOffset + xb, yOffset + yb, z ), QVector3D( 1.0, 1.0, z/zb ) };
        vertices[3] =  { QVector3D( xOffset,      yOffset + yb, z ), QVector3D( 0.0, 1.0, z/zb ) };

        verticesCrosshair[0] = { QVector3D( xOffset,      yOffset + y,  zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[1] = { QVector3D( xOffset + xb, yOffset + y,  zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[2] = { QVector3D( xOffset + x,  yOffset,      zb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[3] = { QVector3D( xOffset + x,  yOffset + yb, zb ), QVector3D( 0.0, 0.0, 0.0 ) };

        yOffset = yb;
        //coronal
        vertices[4] =  { QVector3D( xOffset,      yOffset,      y ), QVector3D( 0.0, y/yb, 0.0 ) };
        vertices[5] =  { QVector3D( xOffset + xb, yOffset,      y ), QVector3D( 1.0, y/yb, 0.0 ) };
        vertices[6] =  { QVector3D( xOffset + xb, yOffset + zb, y ), QVector3D( 1.0, y/yb, 1.0 ) };
        vertices[7] =  { QVector3D( xOffset,      yOffset + zb, y ), QVector3D( 0.0, y/yb, 1.0 ) };

        verticesCrosshair[4] = { QVector3D( xOffset,      yOffset + z,  yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[5] = { QVector3D( xOffset + xb, yOffset + z,  yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[6] = { QVector3D( xOffset + x,  yOffset,      yb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[7] = { QVector3D( xOffset + x,  yOffset + zb, yb ), QVector3D( 0.0, 0.0, 0.0 ) };

        xOffset = xb;
        //sagittal
        vertices[8] =  { QVector3D( xOffset,      yOffset,      x ), QVector3D( x/xb, 1.0, 0.0 ) };
        vertices[9] =  { QVector3D( xOffset + yb, yOffset,      x ), QVector3D( x/xb, 0.0, 0.0 ) };
        vertices[10] = { QVector3D( xOffset + yb, yOffset + zb, x ), QVector3D( x/xb, 0.0, 1.0 ) };
        vertices[11] = { QVector3D( xOffset,      yOffset + zb, x ), QVector3D( x/xb, 1.0, 1.0 ) };

        verticesCrosshair[8] =  { QVector3D( xOffset,          yOffset + z,  xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[9] =  { QVector3D( xOffset + yb,     yOffset + z,  xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[10] = { QVector3D( xOffset + yb - y, yOffset,      xb ), QVector3D( 0.0, 0.0, 0.0 ) };
        verticesCrosshair[11] = { QVector3D( xOffset + yb - y, yOffset + zb, xb ), QVector3D( 0.0, 0.0, 0.0 ) };

        xOffset = 0;
        yOffset = yb;

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

    GLushort crosshairIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    // Transfer index data to VBO 2
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 2 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(GLushort), crosshairIndices, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 3
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(VertexData), verticesCrosshair, GL_STATIC_DRAW );
}

void CombinedNavRenderer::setupTextures()
{
    GLFunctions::setupTextures( m_dataModel );
}

void CombinedNavRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "slice", m_dataModel );
}

void CombinedNavRenderer::draw()
{
    QColor color = model()->data( model()->index( (int)Fn::Global::BACKGROUND_COLOR_COMBINED, 0 ) ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

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

    bool renderCrosshairs = model()->data( model()->index( (int)Fn::Global::RENDER_CROSSHAIRS, 0 ) ).toBool();

    if ( renderCrosshairs )
    {
        GLFunctions::getShader( "crosshair" )->bind();
        GLFunctions::getShader( "crosshair" )->setUniformValue( "mvp_matrix", m_mvpMatrix );
        QColor ccolor = model()->data( model()->index( (int)Fn::Global::CROSSHAIR_COLOR, 0 ) ).value<QColor>();
        GLFunctions::getShader( "crosshair" )->setUniformValue( "u_color", ccolor.redF(), ccolor.greenF(), ccolor.blueF() );
        // Tell OpenGL which VBOs to use
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 2 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = GLFunctions::getShader( "crosshair" )->attributeLocation( "a_position" );
        GLFunctions::getShader( "crosshair" )->enableAttributeArray( vertexLocation );
        glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof( VertexData ), 0 );

        glDrawElements( GL_LINES, 12, GL_UNSIGNED_SHORT, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
}
