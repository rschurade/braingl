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

    m_nx = model()->data( model()->index( (int)Fn::Property::G_MAX_SAGITTAL, 0 ) ).toInt();
    m_ny = model()->data( model()->index( (int)Fn::Property::G_MAX_CORONAL, 0 ) ).toInt();
    m_nz = model()->data( model()->index( (int)Fn::Property::G_MAX_AXIAL, 0 ) ).toInt();

    m_dx = model()->data( model()->index( (int)Fn::Property::G_SLICE_DX, 0 ) ).toFloat();
    m_dy = model()->data( model()->index( (int)Fn::Property::G_SLICE_DY, 0 ) ).toFloat();
    m_dz = model()->data( model()->index( (int)Fn::Property::G_SLICE_DZ, 0 ) ).toFloat();

    float x = m_x * m_dx;
    float y = m_y * m_dy;
    float z = m_z * m_dz;

    float lx = -m_dx / 2.0;
    float ly = -m_dy / 2.0;
    float lz = -m_dz / 2.0;

    float xb = m_nx * m_dx + lx;
    float yb = m_ny * m_dy + ly;
    float zb = m_nz * m_dz + lz;



    if ( m_z != m_zOld || zb != m_zbOld )
    {
        VertexData verticesAxial[] =
        {
            // XXX rearrange quad vertices (1,2,3,4) to triangle strip (1,2,4,3)
            { QVector3D( lx, ly, z ), QVector3D( 0.0, 0.0, ( m_z ) / ( m_nz - 1 ) ) },
            { QVector3D( xb, ly, z ), QVector3D( 1.0, 0.0, ( m_z ) / ( m_nz - 1 ) ) },
            { QVector3D( lx, yb, z ), QVector3D( 0.0, 1.0, ( m_z ) / ( m_nz - 1 ) ) },
            { QVector3D( xb, yb, z ), QVector3D( 1.0, 1.0, ( m_z ) / ( m_nz - 1 ) ) }
        };
        // Transfer vertex data to VBO 1
        glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesAxial, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    if ( m_y != m_yOld || yb != m_ybOld )
    {
        VertexData verticesCoronal[] =
        {
            // XXX rearrange quad vertices (1,2,3,4) to triangle strip (1,2,4,3)
            { QVector3D( lx, y, lz ), QVector3D( 0.0, ( m_y ) / ( m_ny - 1 ), 0.0 ) },
            { QVector3D( xb, y, lz ), QVector3D( 1.0, ( m_y ) / ( m_ny - 1 ), 0.0 ) },
            { QVector3D( lx, y, zb ), QVector3D( 0.0, ( m_y ) / ( m_ny - 1 ), 1.0 ) },
            { QVector3D( xb, y, zb ), QVector3D( 1.0, ( m_y ) / ( m_ny - 1 ), 1.0 ) }
        };

        // Transfer vertex data to VBO 2
        glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCoronal, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    if ( m_x != m_xOld  || xb != m_xbOld )
    {
        VertexData verticesSagittal[] =
        {
            // XXX rearrange quad vertices (1,2,3,4) to triangle strip (1,2,4,3)
            { QVector3D( x, ly, lz ), QVector3D( ( m_x ) / ( m_nx - 1 ), 0.0, 0.0 ) },
            { QVector3D( x, yb, lz ), QVector3D( ( m_x ) / ( m_nx - 1 ), 1.0, 0.0 ) },
            { QVector3D( x, ly, zb ), QVector3D( ( m_x ) / ( m_nx - 1 ), 0.0, 1.0 ) },
            { QVector3D( x, yb, zb ), QVector3D( ( m_x ) / ( m_nx - 1 ), 1.0, 1.0 ) }
        };
        // Transfer vertex data to VBO 3
        glBindBuffer( GL_ARRAY_BUFFER, vbo2 );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesSagittal, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    m_xOld = m_x;
    m_yOld = m_y;
    m_zOld = m_z;
    m_xbOld = xb;
    m_ybOld = yb;
    m_zbOld = zb;
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
    // XXX not in Core/deprecated //glColor4f( 0.0, 0.0, 0.0, 1.0 );

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
    // XXX not in Core/deprecated //glDrawArrays( GL_QUADS, 0, 4 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );    // XXX

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawCoronal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    setShaderVars( target );
    // Draw cube geometry using indices from VBO 1
    // XXX not in Core/deprecated //glDrawArrays( GL_QUADS, 0, 4 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );    // XXX

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawSagittal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo2 );
    setShaderVars( target );
    // Draw cube geometry using indices from VBO 2
    // XXX not in Core/deprecated //glDrawArrays( GL_QUADS, 0, 4 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );    // XXX

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
