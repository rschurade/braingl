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
#include "../../data/datasets/dataset.h"

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

    QPoint p( out.x(), out.z() );
    Models::setGlobal( Fn::Property::G_SAGITTAL_AXIAL, p );
    Models::g()->submit();
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

void NavRendererCoronal::initGeometry()
{
    m_x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toInt();
    m_y = Models::getGlobal( Fn::Property::G_CORONAL ).toInt();
    m_z = Models::getGlobal( Fn::Property::G_AXIAL ).toInt();

    if ( m_xOld != m_x || m_yOld != m_y || m_zOld != m_z )
    {
        QList<Dataset*>dsl = Models::getDatasets( Fn::DatasetType::NIFTI_ANY );

        if ( dsl.size() > 0 )
        {
            Dataset* ds = dsl.first();
            m_nx = ds->properties().get( Fn::Property::D_NX ).toFloat();
            m_ny = ds->properties().get( Fn::Property::D_NY ).toFloat();
            m_nz = ds->properties().get( Fn::Property::D_NZ ).toFloat();

            m_dx = ds->properties().get( Fn::Property::D_DX ).toFloat();
            m_dy = ds->properties().get( Fn::Property::D_DY ).toFloat();
            m_dz = ds->properties().get( Fn::Property::D_DZ ).toFloat();

            m_ax = ds->properties().get( Fn::Property::D_ADJUST_X ).toFloat();
            m_ay = ds->properties().get( Fn::Property::D_ADJUST_Y ).toFloat();
            m_az = ds->properties().get( Fn::Property::D_ADJUST_Z ).toFloat();
        }


        float vertices[] =
        {
            -250.0, m_y, -250.0,
             250.0, m_y, -250.0,
             250.0, m_y,  250.0,
            -250.0, m_y,  250.0
        };

        float verticesCrosshair[] =
        {
           -250.0, m_y - 1.f, m_z,
            250.0, m_y - 1.f, m_z,
            m_x,   m_y - 1.f, -250.0,
            m_x,   m_y - 1.f,  250.0
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
