/*
 * NavRendererSagittal.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "navrenderersagittal.h"

#include <QtCore/QDebug>

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../../data/enums.h"
#include "../gl/glfunctions.h"

NavRendererSagittal::NavRendererSagittal( QString name ) :
    NavRenderer( name )
{
}

NavRendererSagittal::~NavRendererSagittal()
{
    glDeleteBuffers( 4, vboIds );
    delete[] vboIds;
}

void NavRendererSagittal::adjustRatios()
{
    glViewport( 0, 0, m_width, m_height );

    m_ratio = static_cast< float >( m_width ) / static_cast< float >( m_height );

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();

    int boundingbox = qMax ( m_xb * m_xd, qMax( m_yb * m_yd, m_zb * m_zd ) );

    pMatrix.ortho( 0, boundingbox, 0, boundingbox, -3000, 3000 );

    pMatrix.translate( static_cast<float>( m_moveX ) / ( static_cast<float>( m_width ) / boundingbox ),
                       static_cast<float>( m_moveY ) / ( static_cast<float>( m_height ) / boundingbox ), 0 );

    pMatrix.translate( boundingbox / 2, boundingbox / 2, 0 );

    pMatrix.scale( m_zoom );

    if ( m_ratio >= 1.0 )
    {
        pMatrix.scale( 1.0 / m_ratio, 1.0, 1.0 );
    }
    else
    {
        pMatrix.scale( 1.0, m_ratio, 1.0 );
    }

    pMatrix.translate( m_yb * m_yd / -2, m_zb * m_zd / -2, 0 );
    m_mvpMatrix = pMatrix;
}

void NavRendererSagittal::leftMouseDown( int x, int y )
{
    float xf = static_cast<float>( x );
    float yf = static_cast<float>( m_height - y );

    QVector4D test;
    test.setX( ( 2 * xf ) / m_width - 1 );
    test.setY( ( 2 * yf ) / m_height - 1 );
    test.setZ( 1 );
    test.setW( 1 );

    QVector4D out = m_mvpMatrix.inverted() * test;

    int xout = out.x() / m_yd;
    int yout = out.y() / m_zd;

    xout = ( m_yb - 1 ) - xout;
    xout = qMax( 0, qMin( xout, static_cast<int>( m_yb - 1.0 ) ) );
    yout = qMax( 0, qMin( yout, static_cast<int>( m_zb - 1.0 ) ) );

    QModelIndex mi;
    QPoint p( xout, yout );
    mi = model()->index( 0, FNGLOBAL_CORONAL_AXIAL );
    if ( mi.isValid() )
    {
        model()->setData( mi, p, Qt::UserRole );
    }
}

void NavRendererSagittal::initGeometry()
{
    m_x = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toFloat();
    int xi = m_x;
    int yi = m_y;
    int zi = m_z;

    if ( m_xOld != xi || m_yOld != yi || m_zOld != zi )
    {
        m_xb = model()->data( model()->index( 0, FNGLOBAL_MAX_SAGITTAL ), Qt::UserRole ).toFloat();
        m_yb = model()->data( model()->index( 0, FNGLOBAL_MAX_CORONAL ), Qt::UserRole ).toFloat();
        m_zb = model()->data( model()->index( 0, FNGLOBAL_MAX_AXIAL ), Qt::UserRole ).toFloat();

        m_xd = model()->data( model()->index( 0, FNGLOBAL_SLICE_DX ), Qt::UserRole ).toFloat();
        m_yd = model()->data( model()->index( 0, FNGLOBAL_SLICE_DY ), Qt::UserRole ).toFloat();
        m_zd = model()->data( model()->index( 0, FNGLOBAL_SLICE_DZ ), Qt::UserRole ).toFloat();

        float x = m_x * m_xd;
        float y = m_y * m_yd;
        float z = m_z * m_zd;
        //float xb = m_xb * m_xd;
        float yb = m_yb * m_yd;
        float zb = m_zb * m_zd;

        VertexData vertices[] =
        {
            { QVector3D( 0.0, 0.0, x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) },
            { QVector3D( yb,  0.0, x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) },
            { QVector3D( yb,  zb,  x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) },
            { QVector3D( 0.0, zb,  x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) }
        };

        VertexData verticesCrosshair[] =
        {
            { QVector3D( 0.0,  z + m_zd / 2., x + 1. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( yb, z + m_zd / 2., x + 1. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( yb - ( y + m_yd / 2. ),  0.0, x + 1. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( yb - ( y + m_yd / 2. ), zb, x + 1. ), QVector3D( 0.0, 0.0, 0.0 ) }
        };

        // Transfer vertex data to VBO 3
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

        // Transfer vertex data to VBO 4
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCrosshair, GL_STATIC_DRAW );

        m_xOld = xi;
        m_yOld = yi;
        m_zOld = zi;
    }
}

void NavRendererSagittal::draw()
{
    //qDebug() << "nav draw";
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    adjustRatios();

    setupTextures();

    GLFunctions::getShader( "slice" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "slice" )->setUniformValue( "mvp_matrix", m_mvpMatrix );

    initGeometry();

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    bool renderCrosshirs = model()->data( model()->index( 0, FNSETTING_RENDER_CROSSHAIRS ), Qt::UserRole ).toBool();

    if ( renderCrosshirs )
    {
        GLFunctions::getShader( "crosshair" )->bind();
        GLFunctions::getShader( "crosshair" )->setUniformValue( "mvp_matrix", m_mvpMatrix );
        // Tell OpenGL which VBOs to use
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 3 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
        long int offset = 0;
        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = GLFunctions::getShader( "crosshair" )->attributeLocation( "a_position" );
        GLFunctions::getShader( "crosshair" )->enableAttributeArray( vertexLocation );
        glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof( VertexData ), (const void *) offset );
        // Draw cube geometry using indices from VBO 0
        glDrawElements( GL_LINES, 4, GL_UNSIGNED_SHORT, 0 );
    }
}
