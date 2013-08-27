/*
 * vectorglyphrenderer.cpp
 *
 *  Created on: Apr 25, 2013
 *      Author: boettgerj
 */

#include "vectorglyphrenderer.h"

#include "glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>

VectorGlyphRenderer::VectorGlyphRenderer() :
        ObjectRenderer(), vboIds( new GLuint[1] ), ps( new float[1] ), np( 1 ), ao( 14 ), m_pickId( GLFunctions::getPickIndex() )
{

}

VectorGlyphRenderer::~VectorGlyphRenderer()
{
    glDeleteBuffers( 1, &( vboIds[0] ) );
    delete[] ps;
    ps = NULL;
}

void VectorGlyphRenderer::init()
{
    glGenBuffers( 1, vboIds );
}

void VectorGlyphRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props )
{
    setRenderParams(props);

    float alpha = props->get( Fn::Property::D_GLYPH_ALPHA ).toFloat();

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

    QGLShaderProgram* program = GLFunctions::getShader( "vectors" );
    program->bind();

    // Set modelview-projection matrix

    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    //Rotation of the individual glyphs:
    float rotx = props->get( Fn::Property::D_GLYPH_ROT_X ).toFloat();
    QMatrix4x4 rotMatrix;
    rotMatrix.rotate( rotx, 1, 0, 0 );
    float roty = props->get( Fn::Property::D_GLYPH_ROT_Y ).toFloat();
    rotMatrix.rotate( roty, 0, 1, 0 );
    float rotz = props->get( Fn::Property::D_GLYPH_ROT_Z ).toFloat();
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

    program->setUniformValue( "u_colorMode", m_colorMode );
    program->setUniformValue( "u_colormap", m_colormap );
    program->setUniformValue( "u_color", m_color.redF(), m_color.greenF(), m_color.blueF(), 1.0 );
    program->setUniformValue( "u_selectedMin", m_selectedMin );
    program->setUniformValue( "u_selectedMax", m_selectedMax );
    program->setUniformValue( "u_lowerThreshold", m_lowerThreshold );
    program->setUniformValue( "u_upperThreshold", m_upperThreshold );

    float pAlpha = 1.0;
    float blue = (float) ( ( m_pickId ) & 0xFF ) / 255.f;
    float green = (float) ( ( m_pickId >> 8 ) & 0xFF ) / 255.f;
    float red = (float) ( ( m_pickId >> 16 ) & 0xFF ) / 255.f;
    program->setUniformValue( "u_pickColor", red, green, blue, pAlpha );

    float scale = ( mv_matrix * QVector4D( 1, 0, 0, 1 ) ).length();
    program->setUniformValue( "u_scale", scale );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );

    setShaderVars( props );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_POINT_SMOOTH );

    if ( props->get( Fn::Property::D_DRAW_GLYPHS ).toBool() )
    {
        glDrawArrays( GL_LINES, 0, np * 2 );
    }

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void VectorGlyphRenderer::setupTextures()
{

}

void VectorGlyphRenderer::setShaderVars( PropertyGroup* props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "vectors" );

    program->bind();

    program->setUniformValue( "threshold", props->get( Fn::Property::D_THRESHOLD ).toFloat() );
    program->setUniformValue( "radius", props->get( Fn::Property::D_GLYPHRADIUS ).toFloat() );
    program->setUniformValue( "minlength", props->get( Fn::Property::D_MINLENGTH ).toFloat() );

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );
    offset += sizeof(float) * 3;

    int toLocation = program->attributeLocation( "a_to" );
    program->enableAttributeArray( toLocation );
    glVertexAttribPointer( toLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );
    offset += sizeof(float) * 3;

    int valueLocation = program->attributeLocation( "a_value" );
    program->enableAttributeArray( valueLocation );
    glVertexAttribPointer( valueLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );
    offset += sizeof(float) * 1;

    //TODO: throw flag out, or use it for something cool?
    int flagLocation = program->attributeLocation( "a_flag" );
    program->enableAttributeArray( flagLocation );
    glVertexAttribPointer( flagLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );
    offset += sizeof(float) * 1;

    int dgLocation = program->attributeLocation( "dg" );
    program->enableAttributeArray( dgLocation );
    glVertexAttribPointer( dgLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );
    offset += sizeof(float) * 3;

    int dcLocation = program->attributeLocation( "dc" );
    program->enableAttributeArray( dcLocation );
    glVertexAttribPointer( dcLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * ao, (const void *) offset );
}

void VectorGlyphRenderer::initGeometry( float* points, int number )
{
    ps = points;
    np = number;

    qDebug() << "number of points: " << np;

    qDebug() << np * 2 * ao * sizeof(GLfloat);

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );
    //for more than ~50 mio. points (threshold < 0.2), this seems to crash the x-server on the institute workstation...
    glBufferData( GL_ARRAY_BUFFER, np * 2 * ao * sizeof(GLfloat), ps, GL_STATIC_DRAW );
}

void VectorGlyphRenderer::setRenderParams( PropertyGroup* props )
{
    m_colorMode = props->get( Fn::Property::D_GLYPH_COLORMODE ).toInt();
    m_colormap = props->get( Fn::Property::D_COLORMAP ).toInt();
    m_selectedMin = props->get( Fn::Property::D_SELECTED_MIN ).toFloat();
    m_selectedMax = props->get( Fn::Property::D_SELECTED_MAX ).toFloat();
    m_lowerThreshold = props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat();
    m_upperThreshold = props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat();
    m_color = props->get( Fn::Property::D_COLOR ).value<QColor>();
}
