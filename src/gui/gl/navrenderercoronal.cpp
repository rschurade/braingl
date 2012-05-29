/*
 * NavRendererCoronal.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "navrenderercoronal.h"

#include <QtCore/QDebug>

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../gl/glfunctions.h"

NavRendererCoronal::NavRendererCoronal( QString name ) :
    NavRenderer( name )
{
}

NavRendererCoronal::~NavRendererCoronal()
{
    glDeleteBuffers( 4, vboIds );
    delete[] vboIds;
}

void NavRendererCoronal::adjustRatios()
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

    pMatrix.translate( m_xb * m_xd / -2, m_zb * m_zd / -2, 0 );
    m_mvpMatrix = pMatrix;
}

void NavRendererCoronal::leftMouseDown( int x, int y )
{
    float xf = static_cast<float>( x );
    float yf = static_cast<float>( m_height - y );

    QVector4D test;
    test.setX( ( 2 * xf ) / m_width - 1 );
    test.setY( ( 2 * yf ) / m_height - 1 );
    test.setZ( 1 );
    test.setW( 1 );

    QVector4D out = m_mvpMatrix.inverted() * test;

    int xout = out.x() / m_xd;
    int yout = out.y() / m_zd;

    xout = qMax( 0, qMin( xout, static_cast<int>( m_xb ) ) );
    yout = qMax( 0, qMin( yout, static_cast<int>( m_zb ) ) );

    QModelIndex mi;
    QPoint p( xout, yout );
    mi = model()->index( 0, 110 );
    if ( mi.isValid() )
    {
        model()->setData( mi, p, Qt::UserRole );
    }
}

void NavRendererCoronal::initGeometry()
{
    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();

    int xi = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toInt();

    if ( m_xOld != xi || m_yOld != yi || m_zOld != zi )
    {
        m_xb = model()->data( model()->index( 0, 103 ), Qt::UserRole ).toFloat();
        m_yb = model()->data( model()->index( 0, 104 ), Qt::UserRole ).toFloat();
        m_zb = model()->data( model()->index( 0, 105 ), Qt::UserRole ).toFloat();

        m_xd = model()->data( model()->index( 0, 106 ), Qt::UserRole ).toFloat();
        m_yd = model()->data( model()->index( 0, 107 ), Qt::UserRole ).toFloat();
        m_zd = model()->data( model()->index( 0, 108 ), Qt::UserRole ).toFloat();

        float x = m_x * m_xd;
        float y = m_y * m_yd;
        float z = m_z * m_zd;
        float xb = m_xb * m_xd;
        float yb = m_yb * m_yd;
        float zb = m_zb * m_zd;

        VertexData vertices[] =
        {
            { QVector3D( 0.0, 0.0, y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) },
            { QVector3D( xb,  0.0, y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) },
            { QVector3D( xb,  zb,  y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) },
            { QVector3D( 0.0, zb,  y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) }
        };

        VertexData verticesCrosshair[] =
        {
            { QVector3D( 0.0,  z + m_zd / 2., y + 1. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( xb, z + m_zd / 2., y + 1. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( x + m_xd / 2.,  0.0, y + 1. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( x + m_xd / 2., zb, y + 1. ), QVector3D( 0.0, 0.0, 0.0 ) }
        };

        // Transfer vertex data to VBO 2
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

void NavRendererCoronal::draw()
{
    //qDebug() << "nav draw";
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setupTextures();

    adjustRatios();

    m_program->bind();
    // Set modelview-projection matrix
    m_program->setUniformValue( "mvp_matrix", m_mvpMatrix );

    initGeometry();

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    m_crosshairProgram->bind();
    m_crosshairProgram->setUniformValue( "mvp_matrix", m_mvpMatrix );
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 3 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_crosshairProgram->attributeLocation( "a_position" );
    m_crosshairProgram->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof( VertexData ), (const void *) offset );
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_LINES, 4, GL_UNSIGNED_SHORT, 0 );
}
