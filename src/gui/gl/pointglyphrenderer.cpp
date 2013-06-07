/*
 * pointglyphrenderer.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#include "pointglyphrenderer.h"

#include "glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>

#include "qmatrix4x4.h"

PointGlyphRenderer::PointGlyphRenderer() :
        ObjectRenderer(),
        vboIds( new GLuint[1] ),
        ps( new float[1] ),
        np( 1 ),
        ao( 13 ),
        m_pickId( GLFunctions::getPickIndex() )
{

}

PointGlyphRenderer::~PointGlyphRenderer()
{
    glDeleteBuffers( 1, &( vboIds[0] ) );
    delete[] ps;
    ps = NULL;
}

void PointGlyphRenderer::init()
{
    glGenBuffers( 1, vboIds );
}

void PointGlyphRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props )
{
    float alpha = props->get( Fn::Property::GLYPH_ALPHA ).toFloat();

    if ( renderMode == 1 ) // we are drawing opaque objects
    {
        if ( alpha < 1.0 )
        {
            // obviously not opaque
            return;
        }
    }
    else // we are drawing tranparent objects
    {
        if ( !( alpha < 1.0 ) )
        {
            // not transparent
            return;
        }
    }

    QGLShaderProgram* program = GLFunctions::getShader( "points" );
    program->bind();

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    //Rotation of the individual glyphs:
    float rotx = props->get( Fn::Property::GLYPH_ROT_X ).toFloat();
    QMatrix4x4 rotMatrix;
    rotMatrix.rotate( rotx, 1, 0, 0 );
    float roty = props->get( Fn::Property::GLYPH_ROT_Y ).toFloat();
    rotMatrix.rotate( roty, 0, 1, 0 );
    float rotz = props->get( Fn::Property::GLYPH_ROT_Z ).toFloat();
    rotMatrix.rotate( rotz, 0, 0, 1 );
    program->setUniformValue( "rot_matrix", rotMatrix );

    program->setUniformValue( "u_alpha", 1.0f );
    program->setUniformValue( "u_beta", alpha ); //shader uses vec4...
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    float scale = ( mv_matrix * QVector4D( 1, 0, 0, 1 ) ).length();
    program->setUniformValue( "u_scale", scale );

    float pAlpha =  1.0;
    float blue = (float) ( ( m_pickId ) & 0xFF ) / 255.f;
    float green = (float) ( ( m_pickId >> 8 ) & 0xFF ) / 255.f;
    float red = (float) ( ( m_pickId >> 16 ) & 0xFF ) / 255.f;
    program->setUniformValue( "u_pickColor", red, green , blue, pAlpha );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );

    setShaderVars( props );

    glEnable( GL_BLEND );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_POINT_SMOOTH );

    glDrawArrays( GL_POINTS, 0, np );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void PointGlyphRenderer::setupTextures()
{

}

void PointGlyphRenderer::setShaderVars( PropertyGroup* props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "points" );

    program->bind();

    program->setUniformValue( "threshold", props->get( Fn::Property::THRESHOLD ).toFloat() );
    program->setUniformValue( "radius", props->get( Fn::Property::GLYPHRADIUS ).toFloat() );
    program->setUniformValue( "minlength", props->get( Fn::Property::MINLENGTH ).toFloat() );

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );

    offset += sizeof(float) * 3;
    int valueLocation = program->attributeLocation( "a_value" );
    program->enableAttributeArray( valueLocation );
    glVertexAttribPointer( valueLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );

    offset += sizeof(float) * 1;
    int toLocation = program->attributeLocation( "a_to" );
    program->enableAttributeArray( toLocation );
    glVertexAttribPointer( toLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );

    offset += sizeof(float) * 3;
    int dgLocation = program->attributeLocation( "dg" );
    program->enableAttributeArray( dgLocation );
    glVertexAttribPointer( dgLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );

    offset += sizeof(float) * 3;
    int dcLocation = program->attributeLocation( "dc" );
    program->enableAttributeArray( dcLocation );
    glVertexAttribPointer( dcLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );
}

void PointGlyphRenderer::initGeometry( float* points, int number )
{
    ps = points;
    np = number;

    qDebug() << "number of points: " << np;

    qDebug() << np * ao * sizeof(GLfloat);

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );
    //for more than ~50 mio. points (threshold < 0.2), this seems to crash the x-server on the institute workstation...
    glBufferData( GL_ARRAY_BUFFER, np * ao * sizeof(GLfloat), ps, GL_STATIC_DRAW );
}
