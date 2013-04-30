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
        ObjectRenderer(), vboIds( new GLuint[1] ), pies( new float[1] ), np( 1 )
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
    // todo joachim
    if ( renderMode != 1 ) // we are drawing opaque objects
    {
        // obviously not opaque
        return;
    }

    QGLShaderProgram* program = GLFunctions::getShader( "pies" );
    program->bind();

    // Set modelview-projection matrix

    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "p_matrix", p_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    float alpha = props->get( Fn::Property::ALPHA ).toFloat();
    program->setUniformValue( "u_alpha", alpha );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    float scale = ( mv_matrix * QVector4D( 1, 0, 0, 1 ) ).length();
    program->setUniformValue( "u_scale", scale );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );

    setShaderVars( props );

    glShadeModel( GL_FLAT );

    int start = 0;
    qDebug() << "n: " << n;
    for ( int i = 0; i < n; i++ )
    {
        int number = m_numbers->at( i );
        if ( number != 0 )
        {
            glDrawArrays( GL_TRIANGLE_FAN, start, number );
        }
        start += number;
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

    program->setUniformValue( "threshold", props->get( Fn::Property::THRESHOLD ).toFloat() );
    program->setUniformValue( "minlength", props->get( Fn::Property::MINLENGTH ).toFloat() );
    program->setUniformValue( "radius", props->get( Fn::Property::GLYPHRADIUS ).toFloat() );
    program->setUniformValue( "normalization", props->get( Fn::Property::NORMALIZATION ).toFloat() );

    // Tell OpenGL programmable pipeline how to locate vertex position data
    long int offset = 0;
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );
    offset += sizeof(float) * 3;

    int colorLocation = program->attributeLocation( "a_diff" );
    program->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );
    offset += sizeof(float) * 3;

    int nthLocation = program->attributeLocation( "a_nth" );
    program->enableAttributeArray( nthLocation );
    glVertexAttribPointer( nthLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );
    offset += sizeof(float) * 1;

    int coverLocation = program->attributeLocation( "a_cover" );
    program->enableAttributeArray( coverLocation );
    glVertexAttribPointer( coverLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );
    offset += sizeof(float) * 1;
}

void PieGlyphRenderer::initGeometry( QVector<float*>* pieArrays, QVector<int>* numbers )
{
    m_numbers = numbers;
    n = pieArrays->size();
    int numPoints = 0;
    for ( int i = 0; i < n; i++ )
    {
        numPoints += numbers->at( i );
        //qDebug() << numPoints;
    }
    qDebug() << "numPoints: " << numPoints;
    delete[] pies;
    qDebug() << "pies deleted";

    pies = new float[numPoints * 8];
    qDebug() << "new pies made";
    numPoints = 0;
    for ( int i = 0; i < n; i++ )
    {
        float* ps = pieArrays->at( i );
        //qDebug() << "numbers at: " << i << " is: " << numbers->at(i);
        if ( numbers->at(i) != 0  )
        {
            //TODO: Deal with first and last point here?
            np = numbers->at( i );
            for ( int i2 = 0; i2 < np; i2++ )
            {
                pies[( numPoints + i2 ) * 8] = ps[i2 * 8];
                pies[( numPoints + i2 ) * 8 + 1] = ps[i2 * 8 + 1];
                pies[( numPoints + i2 ) * 8 + 2] = ps[i2 * 8 + 2];
                pies[( numPoints + i2 ) * 8 + 3] = ps[i2 * 8 + 3];
                pies[( numPoints + i2 ) * 8 + 4] = ps[i2 * 8 + 4];
                pies[( numPoints + i2 ) * 8 + 5] = ps[i2 * 8 + 5];
                pies[( numPoints + i2 ) * 8 + 6] = ps[i2 * 8 + 6];
                pies[( numPoints + i2 ) * 8 + 7] = ps[i2 * 8 + 7];
            }
            numPoints += np;
        }
    }
    qDebug() << "pre-bind";
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[0] );
    //two points for every connection, each with value and +1/-1 flag for node / other node:
    qDebug() << "pre-bufferData";
    glBufferData( GL_ARRAY_BUFFER, numPoints * 8 * sizeof(GLfloat), pies, GL_STATIC_DRAW );
    qDebug() << "post-bufferData";
}

