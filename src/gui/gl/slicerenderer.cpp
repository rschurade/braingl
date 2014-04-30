/*
 * slicerenderer.cpp
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */
#include "slicerenderer.h"

#include "glfunctions.h"
#include "../../data/enums.h"
#include "../../data/models.h"

#include "../../data/datasets/dataset.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

SliceRenderer::SliceRenderer() :
    ObjectRenderer(),
    vbo0( 0 ),
    vbo1( 0 ),
    vbo2( 0 ){
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
    float maxDim = GLFunctions::maxDim;

    QList< int > tl = GLFunctions::getTextureIndexes( "maingl" );


    float xs = model()->data( model()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toFloat();
    float ys = model()->data( model()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toFloat();
    float zs = model()->data( model()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toFloat();
    float dx = model()->data( model()->index( (int)Fn::Property::G_SLICE_DX, 0 ) ).toFloat();
    float dy = model()->data( model()->index( (int)Fn::Property::G_SLICE_DY, 0 ) ).toFloat();
    float dz = model()->data( model()->index( (int)Fn::Property::G_SLICE_DZ, 0 ) ).toFloat();

    float x = xs * dx;
    float y = ys * dy;
    float z = zs * dz;

    float lx = -maxDim;
    float ly = -maxDim;
    float lz = -maxDim;

    float xb = maxDim;
    float yb = maxDim;
    float zb = maxDim;


    float verticesAxial[] =
    {
        lx, ly, z,
        xb, ly, z,
        lx, yb, z,
        xb, yb, z
    };
    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
    glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(float), verticesAxial, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );



    float verticesCoronal[] =
    {
        lx, y, lz,
        xb, y, lz,
        lx, y, zb,
        xb, y, zb
    };

    // Transfer vertex data to VBO 2
    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(float), verticesCoronal, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    float verticesSagittal[] =
    {
        x, ly, lz,
        x, yb, lz,
        x, ly, zb,
        x, yb, zb
    };
    // Transfer vertex data to VBO 3
    glBindBuffer( GL_ARRAY_BUFFER, vbo2 );
    glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(float), verticesSagittal, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::setupTextures()
{
    GLFunctions::setupTextures();
}

void SliceRenderer::setShaderVars( QString target )
{
    QGLShaderProgram* program = GLFunctions::getShader( "slice" );
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0 );
    GLFunctions::setTextureUniforms( program, target );
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
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawCoronal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    setShaderVars( target );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawSagittal( QString target )
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo2 );
    setShaderVars( target );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
