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
    vboIds( new GLuint[ 4 ] ),
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
    glDeleteBuffers( 4, vboIds );
    delete[] vboIds;
}

void SliceRenderer::init()
{
    glGenBuffers( 4, vboIds );

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    initGeometry();
}

void SliceRenderer::initGeometry()
{
    m_x = model()->data( model()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat();
    m_y = model()->data( model()->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat();
    m_z = model()->data( model()->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat();
    int xi = m_x;
    int yi = m_y;
    int zi = m_z;

    m_nx = model()->data( model()->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toInt();
    m_ny = model()->data( model()->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toInt();
    m_nz = model()->data( model()->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toInt();
    int xbi = m_nx;
    int ybi = m_ny;
    int zbi = m_nz;

    m_dx = model()->data( model()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    m_dy = model()->data( model()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    m_dz = model()->data( model()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

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
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesAxial, GL_STATIC_DRAW );
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
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCoronal, GL_STATIC_DRAW );
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
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesSagittal, GL_STATIC_DRAW );
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
    if ( model()->data( model()->index( (int)Fn::Global::SHOW_AXIAL, 0 ) ).toBool() )
    {
        float blue =  (float)(( 1 ) & 0xFF) / 255.f;
        GLFunctions::getShader( "slice" )->setUniformValue( "u_pickColor", red, green , blue, pAlpha );
        drawAxial( target );
    }
    if ( model()->data( model()->index( (int)Fn::Global::SHOW_CORONAL, 0 ) ).toBool() )
    {
        float blue =  (float)(( 2 ) & 0xFF) / 255.f;
        GLFunctions::getShader( "slice" )->setUniformValue( "u_pickColor", red, green , blue, pAlpha );
        drawCoronal( target );
    }
    if ( model()->data( model()->index( (int)Fn::Global::SHOW_SAGITTAL, 0 ) ).toBool() )
    {
        float blue =  (float)(( 3 ) & 0xFF) / 255.f;
        GLFunctions::getShader( "slice" )->setUniformValue( "u_pickColor", red, green , blue, pAlpha );
        drawSagittal( target );
    }
}

void SliceRenderer::drawAxial( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );

    setShaderVars( target );

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawCoronal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    setShaderVars( target );
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawSagittal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    setShaderVars( target );
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
