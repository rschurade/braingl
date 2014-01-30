/*
 * pieglyphrenderer.cpp
 *
 *  Created on: Apr 29, 2013
 *      Author: boettgerj
 */

#include "pieglyphrenderer.h"

#include "glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>

PieGlyphRenderer::PieGlyphRenderer() :
        ObjectRenderer(), vboIds( new GLuint[1] ), pies( new float[1] ), np( 1 ), m_pickId( GLFunctions::getPickIndex() )
{

}

PieGlyphRenderer::~PieGlyphRenderer()
{
    qDebug() << "pie renderer delete";
    glDeleteBuffers( 1, &( vboIds[0] ) );
    delete[] pies;
    pies = NULL;
}

void PieGlyphRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props )
{
    float alpha = props->get( Fn::Property::D_GLYPH_ALPHA ).toFloat();
    int colorMode = props->get( Fn::Property::D_GLYPH_COLORMODE ).toInt();
    int colormap = props->get( Fn::Property::D_COLORMAP ).toInt();
    float selectedMin = props->get( Fn::Property::D_SELECTED_MIN ).toFloat();
    float selectedMax = props->get( Fn::Property::D_SELECTED_MAX ).toFloat();
    float lowerThreshold = props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat();
    float upperThreshold = props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat();
    QColor color = props->get( Fn::Property::D_COLOR ).value<QColor>();

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

    QGLShaderProgram* program = GLFunctions::getShader( "pies" );
    program->bind();

    // Set modelview-projection matrix

    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "p_matrix", p_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    program->setUniformValue( "u_alpha", alpha );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    program->setUniformValue( "u_colorMode", colorMode );
    program->setUniformValue( "u_colormap", colormap );
    program->setUniformValue( "u_color", color.redF(), color.greenF(), color.blueF(), 1.0 );
    program->setUniformValue( "u_selectedMin", selectedMin );
    program->setUniformValue( "u_selectedMax", selectedMax );
    program->setUniformValue( "u_lowerThreshold", lowerThreshold );
    program->setUniformValue( "u_upperThreshold", upperThreshold );

    float pAlpha = 1.0;
    float blue = (float) ( ( m_pickId ) & 0xFF ) / 255.f;
    float green = (float) ( ( m_pickId >> 8 ) & 0xFF ) / 255.f;
    float red = (float) ( ( m_pickId >> 16 ) & 0xFF ) / 255.f;
    program->setUniformValue( "u_pickColor", red, green, blue, pAlpha );

    float scale = (p_matrix * QVector4D( 1, 0, 0, 1 ) ).length();
    program->setUniformValue( "u_scale", scale );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );

    setShaderVars( props );

    glShadeModel( GL_FLAT );

    int start = 0;
    if ( props->get( Fn::Property::D_DRAW_GLYPHS ).toBool() && (np > 0) )
    {
        for ( int i = 0; i < n; i++ )
        {
            int number = m_numbers->at( i );
            if ( number != 0 )
            {
                number += 2;
                glDrawArrays( GL_TRIANGLE_FAN, start, number );
            }
            start += number;
        }
    }

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void PieGlyphRenderer::init()
{
    glGenBuffers( 1, vboIds );
}

void PieGlyphRenderer::setupTextures()
{

}

void PieGlyphRenderer::setShaderVars( PropertyGroup* props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "pies" );

    program->bind();

    program->setUniformValue( "threshold", props->get( Fn::Property::D_THRESHOLD ).toFloat() );
    program->setUniformValue( "minlength", props->get( Fn::Property::D_MINLENGTH ).toFloat() );
    program->setUniformValue( "radius", props->get( Fn::Property::D_GLYPHRADIUS ).toFloat() );
    program->setUniformValue( "normalization", props->get( Fn::Property::D_NORMALIZATION ).toFloat() );
    program->setUniformValue( "u_maxNodeCount", (float)m_maxNodeCount );

    int elementSize = 9;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    intptr_t offset = 0;
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * elementSize, (const void *) offset );
    offset += sizeof(float) * 3;

    int colorLocation = program->attributeLocation( "a_diff" );
    program->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * elementSize, (const void *) offset );
    offset += sizeof(float) * 3;

    int nthLocation = program->attributeLocation( "a_nth" );
    program->enableAttributeArray( nthLocation );
    glVertexAttribPointer( nthLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * elementSize, (const void *) offset );
    offset += sizeof(float) * 1;

    int coverLocation = program->attributeLocation( "a_cover" );
    program->enableAttributeArray( coverLocation );
    glVertexAttribPointer( coverLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * elementSize, (const void *) offset );
    offset += sizeof(float) * 1;

    int valueLocation = program->attributeLocation( "a_value" );
    program->enableAttributeArray( valueLocation );
    glVertexAttribPointer( valueLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * elementSize, (const void *) offset );
    offset += sizeof(float) * 1;
}

void PieGlyphRenderer::initGeometry( std::vector<float*>* pieArrays, std::vector<int>* numbers, int maxNodeCount )
{
    m_numbers = numbers;
    m_maxNodeCount = maxNodeCount;
    qDebug() << "maxNodeCount: " << m_maxNodeCount;
    n = pieArrays->size();
    int numPoints = 0;
    for ( int i = 0; i < n; i++ )
    {
        if ( numbers->at( i ) != 0 )
            numPoints += numbers->at( i ) + 2; // +2 for middle and closing point
        //qDebug() << numPoints;
    }
    qDebug() << "numPoints: " << numPoints;
    delete[] pies;
    qDebug() << "pies deleted";

    int elementSize = 9;

    pies = new float[numPoints * elementSize];
    qDebug() << "new pies made";
    numPoints = 0;
    for ( int i = 0; i < n; i++ )  //for all pie charts:
    {
        float* ps = pieArrays->at( i );
        //qDebug() << "numbers at: " << i << " is: " << numbers->at(i);
        if ( numbers->at( i ) != 0 )
        {
            //first point:
            pies[( numPoints ) * elementSize] = ps[0];
            pies[( numPoints ) * elementSize + 1] = ps[1];
            pies[( numPoints ) * elementSize + 2] = ps[2];
            pies[( numPoints ) * elementSize + 3] = ps[3];
            pies[( numPoints ) * elementSize + 4] = ps[4];
            pies[( numPoints ) * elementSize + 5] = ps[5];
            pies[( numPoints ) * elementSize + 6] = -1;
            pies[( numPoints ) * elementSize + 7] = ps[7];
            pies[( numPoints ) * elementSize + 8] = ps[8];

            np = numbers->at( i );
            for ( int i2 = 0; i2 < np; ++i2 )
            {
                pies[( numPoints + i2 + 1 ) * elementSize] = ps[i2 * elementSize];
                pies[( numPoints + i2 + 1 ) * elementSize + 1] = ps[i2 * elementSize + 1];
                pies[( numPoints + i2 + 1 ) * elementSize + 2] = ps[i2 * elementSize + 2];
                pies[( numPoints + i2 + 1 ) * elementSize + 3] = ps[i2 * elementSize + 3];
                pies[( numPoints + i2 + 1 ) * elementSize + 4] = ps[i2 * elementSize + 4];
                pies[( numPoints + i2 + 1 ) * elementSize + 5] = ps[i2 * elementSize + 5];
                pies[( numPoints + i2 + 1 ) * elementSize + 6] = ps[i2 * elementSize + 6];
                pies[( numPoints + i2 + 1 ) * elementSize + 7] = ps[i2 * elementSize + 7];
                pies[( numPoints + i2 + 1 ) * elementSize + 8] = ps[i2 * elementSize + 8];
            }

            //closing point:
            pies[( numPoints + np + 1 ) * elementSize] = ps[0];
            pies[( numPoints + np + 1 ) * elementSize + 1] = ps[1];
            pies[( numPoints + np + 1 ) * elementSize + 2] = ps[2];
            pies[( numPoints + np + 1 ) * elementSize + 3] = ps[3];
            pies[( numPoints + np + 1 ) * elementSize + 4] = ps[4];
            pies[( numPoints + np + 1 ) * elementSize + 5] = ps[5];
            pies[( numPoints + np + 1 ) * elementSize + 6] = ps[6];
            pies[( numPoints + np + 1 ) * elementSize + 7] = ps[7];
            pies[( numPoints + np + 1 ) * elementSize + 8] = ps[8];
            numPoints += np + 2;
        }
    }
    qDebug() << "pre-bind";
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );
    //two points for every connection, each with value and +1/-1 flag for node / other node:
    qDebug() << "pre-bufferData";
    glBufferData( GL_ARRAY_BUFFER, numPoints * elementSize * sizeof(GLfloat), pies, GL_STATIC_DRAW );
    qDebug() << "post-bufferData";
}

