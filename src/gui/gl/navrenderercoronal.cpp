/*
 * NavRendererCoronal.cpp
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */
#include "navrenderercoronal.h"
#include "glfunctions.h"
#include "../../data/enums.h"
#include "../../data/models.h"

#include <QDebug>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#include <cmath>

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
    m_pMatrix.setToIdentity();

    int boundingbox = qMax ( m_nx * m_dx, qMax( m_ny * m_dy, m_nz * m_dz ) );

    m_pMatrix.ortho( 0, boundingbox, 0, boundingbox, -3000, 3000 );

    m_pMatrix.translate( static_cast<float>( m_moveX ) / ( static_cast<float>( m_width ) / boundingbox ),
                       static_cast<float>( m_moveY ) / ( static_cast<float>( m_height ) / boundingbox ), 0 );

    m_pMatrix.translate( boundingbox / 2, boundingbox / 2, 0 );

    m_pMatrix.scale( m_zoom );

    if ( m_ratio >= 1.0 )
    {
        m_pMatrix.scale( 1.0 / m_ratio, 1.0, 1.0 );
    }
    else
    {
        m_pMatrix.scale( 1.0, m_ratio, 1.0 );
    }

    m_pMatrix.translate( m_nx * m_dx / -2, m_nz * m_dz / -2, 0 );

    QMatrix4x4 vMatrix;
    vMatrix.setToIdentity();

    QQuaternion rotx( sqrt(0.5), 0, 0, sqrt(0.5) );
    QQuaternion rotz( 0, 0, sqrt(0.5), sqrt(0.5) );

    vMatrix.rotate( rotz );
    vMatrix.rotate( rotx );
    vMatrix.rotate( rotx );

    m_mvpMatrix = m_pMatrix * vMatrix;
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

    QVector4D out = m_pMatrix.inverted() * test;

    int xout = out.x() / m_dx;
    int yout = out.y() / m_dz;

    xout = qMax( 0, qMin( xout, static_cast<int>( m_nx - 1.0 ) ) );
    yout = qMax( 0, qMin( yout, static_cast<int>( m_nz - 1.0 ) ) );

    QPoint p( xout, yout );
    Models::setGlobal( Fn::Property::G_SAGITTAL_AXIAL, p );
    Models::g()->submit();
}

void NavRendererCoronal::initGeometry()
{
    m_x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toInt();
    m_y = Models::getGlobal( Fn::Property::G_CORONAL ).toInt();
    m_z = Models::getGlobal( Fn::Property::G_AXIAL ).toInt();

    if ( m_xOld != m_x || m_yOld != m_y || m_zOld != m_z )
    {
        m_nx = 160; //Models::getGlobal( Fn::Property::G_MAX_SAGITTAL ).toInt();
        m_ny = 200; //Models::getGlobal( Fn::Property::G_MAX_CORONAL ).toInt();
        m_nz = 160; //Models::getGlobal( Fn::Property::G_MAX_AXIAL ).toInt();

        m_dx = 1.0; //Models::getGlobal( Fn::Property::G_SLICE_DX ).toFloat();
        m_dy = 1.0; //Models::getGlobal( Fn::Property::G_SLICE_DY ).toFloat();
        m_dz = 1.0; //Models::getGlobal( Fn::Property::G_SLICE_DZ ).toFloat();

        float x = m_x * m_dx + m_dx / 2.0;
        float y = m_y * m_dy + m_dy / 2.0;
        float z = m_z * m_dz + m_dz / 2.0;
        float xb = m_nx * m_dx;
        //float yb = m_ny * m_dy;
        float zb = m_nz * m_dz;

        float vertices[] =
        {
            0.0, y, 0.0,
            xb,  y, 0.0,
            xb,  y, zb,
            0.0, y, zb
        };

        float verticesCrosshair[] =
        {
            0.0, y - 1.f, z,
            xb,  y - 1.f, z,
            x,   y - 1.f, 0.0,
            x,   y - 1.f, zb
        };

        // Transfer vertex data to VBO 2
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

        // Transfer vertex data to VBO 4
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCrosshair, GL_STATIC_DRAW );

        m_xOld = m_x;
        m_yOld = m_y;
        m_zOld = m_z;
    }
}

void NavRendererCoronal::draw()
{
    QColor color = Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_NAV3 ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

    //qDebug() << "nav draw";
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setupTextures();

    adjustRatios();

    GLFunctions::getShader( "slice" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "slice" )->setUniformValue( "mvp_matrix", m_mvpMatrix );
    GLFunctions::getShader( "slice" )->setUniformValue( "u_renderMode", 0 );

    initGeometry();

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    bool renderCrosshirs = Models::getGlobal( Fn::Property::G_RENDER_CROSSHAIRS ).toBool();

    if ( renderCrosshirs )
    {
        GLFunctions::getShader( "crosshair" )->bind();
        GLFunctions::getShader( "crosshair" )->setUniformValue( "mvp_matrix", m_mvpMatrix );
        QColor ccolor = Models::getGlobal( Fn::Property::G_CROSSHAIR_COLOR ).value<QColor>();
        GLFunctions::getShader( "crosshair" )->setUniformValue( "u_color", ccolor.redF(), ccolor.greenF(), ccolor.blueF(), 1.0f );
        // Tell OpenGL which VBOs to use
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 3 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = GLFunctions::getShader( "crosshair" )->attributeLocation( "a_position" );
        GLFunctions::getShader( "crosshair" )->enableAttributeArray( vertexLocation );
        glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), 0 );
        // Draw cube geometry using indices from VBO 0
        glDrawElements( GL_LINES, 4, GL_UNSIGNED_SHORT, 0 );
    }
}
