/*
 * slicerenderer.cpp
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */
#include "slicerenderer.h"

#include "glfunctions.h"
#include "../../data/enums.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

SliceRenderer::SliceRenderer() :
    ObjectRenderer(),
    vbo0( 0 ),
    vbo1( 0 ),
    vbo2( 0 ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_nx( 1. ),
    m_ny( 1. ),
    m_nz( 1. ),
    m_dx( 1. ),
    m_dy( 1. ),
    m_dz( 1. ),
    m_xOld( -1 ),
    m_yOld( -1 ),
    m_zOld( -1 ),
    m_xbOld( -1 ),
    m_ybOld( -1 ),
    m_zbOld( -1 )
{
}

SliceRenderer::~SliceRenderer()
{
    glDeleteBuffers( 1, &vbo0 );
    glDeleteBuffers( 1, &vbo1 );
    glDeleteBuffers( 1, &vbo2 );
}

void SliceRenderer::init()
{
    glGenBuffers( 1, &vbo0 );
    glGenBuffers( 1, &vbo1 );
    glGenBuffers( 1, &vbo2 );

    initGeometry();
}

void SliceRenderer::initGeometry()
{
    m_x = model()->data( model()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toFloat();
    m_y = model()->data( model()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toFloat();
    m_z = model()->data( model()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toFloat();
    int xi = m_x;
    int yi = m_y;
    int zi = m_z;

    m_nx = model()->data( model()->index( (int)Fn::Property::G_MAX_SAGITTAL, 0 ) ).toInt();
    m_ny = model()->data( model()->index( (int)Fn::Property::G_MAX_CORONAL, 0 ) ).toInt();
    m_nz = model()->data( model()->index( (int)Fn::Property::G_MAX_AXIAL, 0 ) ).toInt();
    int xbi = m_nx;
    int ybi = m_ny;
    int zbi = m_nz;

    m_dx = model()->data( model()->index( (int)Fn::Property::G_SLICE_DX, 0 ) ).toFloat();
    m_dy = model()->data( model()->index( (int)Fn::Property::G_SLICE_DY, 0 ) ).toFloat();
    m_dz = model()->data( model()->index( (int)Fn::Property::G_SLICE_DZ, 0 ) ).toFloat();

    float x = m_x * m_dx + m_dx / 2.0;
    float y = m_y * m_dy + m_dy / 2.0;
    float z = m_z * m_dz + m_dz / 2.0;
    float xb = m_nx * m_dx;
    float yb = m_ny * m_dy;
    float zb = m_nz * m_dz;

    if ( zi != m_zOld || zbi != m_zbOld )
    {
        VertexData verticesAxial[] =
        {
            { QVector3D( 0.0, 0.0, z ), QVector3D( 0.0, 0.0, ( m_z ) / ( m_nz - 1 ) ) },
            { QVector3D( xb,  0.0, z ), QVector3D( 1.0, 0.0, ( m_z ) / ( m_nz - 1 ) ) },
            { QVector3D( xb,  yb,  z ), QVector3D( 1.0, 1.0, ( m_z ) / ( m_nz - 1 ) ) },
            { QVector3D( 0.0, yb,  z ), QVector3D( 0.0, 1.0, ( m_z ) / ( m_nz - 1 ) ) }
        };
        // Transfer vertex data to VBO 1
        glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesAxial, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    if ( yi != m_yOld || ybi != m_ybOld )
    {
        VertexData verticesCoronal[] =
        {
            { QVector3D( 0.0, y, 0.0 ), QVector3D( 0.0, ( m_y ) / ( m_ny - 1 ), 0.0 ) },
            { QVector3D( xb,  y, 0.0 ), QVector3D( 1.0, ( m_y ) / ( m_ny - 1 ), 0.0 ) },
            { QVector3D( xb,  y, zb  ), QVector3D( 1.0, ( m_y ) / ( m_ny - 1 ), 1.0 ) },
            { QVector3D( 0.0, y, zb  ), QVector3D( 0.0, ( m_y ) / ( m_ny - 1 ), 1.0 ) }
        };

        // Transfer vertex data to VBO 2
        glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCoronal, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    if ( xi != m_xOld  || xbi != m_xbOld )
    {
        VertexData verticesSagittal[] =
        {
            { QVector3D( x, 0.0, 0.0 ), QVector3D( ( m_x ) / ( m_nx - 1 ), 0.0, 0.0 ) },
            { QVector3D( x, yb,  0.0 ), QVector3D( ( m_x ) / ( m_nx - 1 ), 1.0, 0.0 ) },
            { QVector3D( x, yb,  zb  ), QVector3D( ( m_x ) / ( m_nx - 1 ), 1.0, 1.0 ) },
            { QVector3D( x, 0.0, zb  ), QVector3D( ( m_x ) / ( m_nx - 1 ), 0.0, 1.0 ) }
        };
        // Transfer vertex data to VBO 3
        glBindBuffer( GL_ARRAY_BUFFER, vbo2 );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesSagittal, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    m_xOld = xi;
    m_yOld = yi;
    m_zOld = zi;
    m_xbOld = xbi;
    m_ybOld = ybi;
    m_zbOld = zbi;
}

void SliceRenderer::setupTextures()
{
    GLFunctions::setupTextures();
}

void SliceRenderer::setShaderVars( QString target )
{
    QGLShaderProgram* program = GLFunctions::getShader( "slice" );
    GLFunctions::setShaderVarsSlice( program, target );
}

void SliceRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, QString target )
{
    float alpha = GLFunctions::sliceAlpha[target];

    switch ( renderMode )
    {
        case 0:
            break;
        case 1:
        {
            if ( alpha < 1.0 ) // obviously not opaque
            {
                return;
            }
            break;
        }
        default:
        {
            if ( alpha == 1.0  ) // not transparent
            {
                return;
            }
            break;
        }
    }

    if ( !GLFunctions::setupTextures() )
    {
        return;
    }
    //qDebug() << "main gl draw, renderMode:" << renderMode;
    glColor4f( 0.0, 0.0, 0.0, 1.0 );

    QGLShaderProgram* program = GLFunctions::getShader( "slice" );

    program->bind();
    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );

    program->setUniformValue( "u_alpha", alpha );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    float pAlpha =  1.0;
    float green = 0.0f;
    float red =   0.0f;

    initGeometry();
    if ( model()->data( model()->index( (int)Fn::Property::G_SHOW_AXIAL, 0 ) ).toBool() )
    {
        float blue =  (float)(( 1 ) & 0xFF) / 255.f;
        GLFunctions::getShader( "slice" )->setUniformValue( "u_pickColor", red, green , blue, pAlpha );
        drawAxial( target );
    }
    if ( model()->data( model()->index( (int)Fn::Property::G_SHOW_CORONAL, 0 ) ).toBool() )
    {
        float blue =  (float)(( 2 ) & 0xFF) / 255.f;
        GLFunctions::getShader( "slice" )->setUniformValue( "u_pickColor", red, green , blue, pAlpha );
        drawCoronal( target );
    }
    if ( model()->data( model()->index( (int)Fn::Property::G_SHOW_SAGITTAL, 0 ) ).toBool() )
    {
        float blue =  (float)(( 3 ) & 0xFF) / 255.f;
        GLFunctions::getShader( "slice" )->setUniformValue( "u_pickColor", red, green , blue, pAlpha );
        drawSagittal( target );
    }
}

void SliceRenderer::drawAxial( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo0 );

    setShaderVars( target );

    // Draw cube geometry using indices from VBO 0
    glDrawArrays( GL_QUADS, 0, 4 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawCoronal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    setShaderVars( target );
    // Draw cube geometry using indices from VBO 1
    glDrawArrays( GL_QUADS, 0, 4 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawSagittal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo2 );
    setShaderVars( target );
    // Draw cube geometry using indices from VBO 2
    glDrawArrays( GL_QUADS, 0, 4 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
