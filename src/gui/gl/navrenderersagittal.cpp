/*
 * NavRendererSagittal.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "navrenderersagittal.h"
#include "glfunctions.h"
#include "../../data/enums.h"

#include <QtCore/QDebug>
#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>




NavRendererSagittal::NavRendererSagittal(  QAbstractItemModel* dataModel, QString name ) :
    NavRenderer( dataModel, name )
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
    mi = model()->index( (int)Fn::Global::CORONAL_AXIAL, 0 );
    if ( mi.isValid() )
    {
        model()->setData( mi, p );
    }
}

void NavRendererSagittal::initGeometry()
{
    m_x = model()->data( model()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat();
    m_y = model()->data( model()->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat();
    m_z = model()->data( model()->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat();
    int xi = m_x;
    int yi = m_y;
    int zi = m_z;

    if ( m_xOld != xi || m_yOld != yi || m_zOld != zi )
    {
        m_xb = model()->data( model()->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toFloat();
        m_yb = model()->data( model()->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toFloat();
        m_zb = model()->data( model()->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toFloat();

        m_xd = model()->data( model()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
        m_yd = model()->data( model()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
        m_zd = model()->data( model()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

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
    QColor color = model()->data( model()->index( (int)Fn::Global::BACKGROUND_COLOR_NAV2, 0 ) ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

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

    bool renderCrosshirs = model()->data( model()->index( (int)Fn::Global::RENDER_CROSSHAIRS, 0 ) ).toBool();

    if ( renderCrosshirs )
    {
        GLFunctions::getShader( "crosshair" )->bind();
        GLFunctions::getShader( "crosshair" )->setUniformValue( "mvp_matrix", m_mvpMatrix );
        QColor ccolor = model()->data( model()->index( (int)Fn::Global::CROSSHAIR_COLOR, 0 ) ).value<QColor>();
        GLFunctions::getShader( "crosshair" )->setUniformValue( "u_color", ccolor.redF(), ccolor.greenF(), ccolor.blueF() );
        // Tell OpenGL which VBOs to use
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 3 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = GLFunctions::getShader( "crosshair" )->attributeLocation( "a_position" );
        GLFunctions::getShader( "crosshair" )->enableAttributeArray( vertexLocation );
        glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof( VertexData ), 0 );
        // Draw cube geometry using indices from VBO 0
        glDrawElements( GL_LINES, 4, GL_UNSIGNED_SHORT, 0 );
    }
}
