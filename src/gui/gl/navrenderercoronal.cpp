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
    m_ratio = static_cast< float >( m_width ) / static_cast< float >( m_height );

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();

    m_xoff = 0;
    m_yoff = 0;

    float textureRatio = m_xb / m_zb;
    float mult = m_ratio / textureRatio;
    if ( mult >= 1.0 )
    {
        pMatrix.ortho( 0, m_xb, 0, m_zb, -3000, 3000 );

        m_xoff = ( m_width - ( m_width / mult ) ) / 2;
        glViewport( m_xoff, 0, m_width - ( 2* m_xoff ) , m_height );
    }
    else
    {
        pMatrix.ortho( 0, m_xb, 0, m_zb, -3000, 3000 );

        m_yoff = ( m_height - ( m_height * mult ) ) / 2;
        glViewport( 0, m_yoff, m_width, m_height - ( 2 * m_yoff ) );
    }

    m_mvpMatrix = pMatrix;
}

void NavRendererCoronal::leftMouseDown( int x, int y )
{
    y = m_height - y;
    float adjustx = x - m_xoff;
    float adjusty = y - m_yoff;
    float xmult = adjustx / ( m_width - ( 2 * m_xoff ) );
    float ymult = adjusty / ( m_height - ( 2 * m_yoff ) );
    int xout = m_xb * xmult / m_xd;
    int yout = m_zb * ymult / m_zd;

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

        m_x *= m_xd;
        m_y *= m_yd;
        m_z *= m_zd;
        m_xb *= m_xd;
        m_yb *= m_yd;
        m_zb *= m_zd;

        VertexData vertices[] =
        {
            { QVector3D( 0.0,  0.0,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, 0.0,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, m_zb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) },
            { QVector3D( 0.0,  m_zb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) }
        };

        VertexData verticesCrosshair[] =
        {
            { QVector3D( 0.0,  m_z + m_zd / 2., 1000. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( m_xb, m_z + m_zd / 2., 1000. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( m_x + m_xd / 2.,  0.0, 1000. ), QVector3D( 0.0, 0.0, 0.0 ) },
            { QVector3D( m_x + m_xd / 2., m_zb, 1000. ), QVector3D( 0.0, 0.0, 0.0 ) }
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
