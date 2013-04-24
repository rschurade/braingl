/*
 * pointglyphrenderer.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#include "pointglyphrenderer.h"

#include "glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>

PointGlyphRenderer::PointGlyphRenderer() :
        ObjectRenderer(),
        vboIds( new GLuint[1] ),
        ps( new float[1] ),
        np( 1 ),
        ao( 13 )
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

void PointGlyphRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, PropertyGroup* props )
{
    qDebug() << "begin glyphrenderer draw";

    GLFunctions::getShader( "points" )->bind();

    qDebug() << "program bound";

    // Set modelview-projection matrix
    GLFunctions::getShader( "points" )->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    GLFunctions::getShader( "points" )->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    qDebug() << "matrices set";

    float scale = (mv_matrix*QVector4D(1,0,0,1)).length();
    GLFunctions::getShader( "points" )->setUniformValue( "u_scale", scale );

    qDebug() << "before bind";

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );

    qDebug() << "before vars";

    setShaderVars(props);

    qDebug() << "after vars";

    glShadeModel(GL_SMOOTH);
    glEnable(GL_POINT_SMOOTH);

    qDebug() << "before draw arrays";

    glDrawArrays( GL_POINTS, 0, np );

    qDebug() << "after draw arrays";

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

}

void PointGlyphRenderer::setupTextures()
{

}


void PointGlyphRenderer::setShaderVars(PropertyGroup* props)
{
    QGLShaderProgram* program = GLFunctions::getShader( "points" );

    program->bind();

    program->setUniformValue("threshold",props->get(Fn::Property::THRESHOLD).toFloat());
    program->setUniformValue("radius",props->get(Fn::Property::GLYPHRADIUS).toFloat());
    program->setUniformValue("minlength",props->get(Fn::Property::MINLENGTH).toFloat());

    long int offset = 0;
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

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    //for more than ~50 mio. points (threshold < 0.2), this seems to crash the x-server on the institute workstation...
    glBufferData( GL_ARRAY_BUFFER,  np * ao * sizeof(GLfloat), ps , GL_STATIC_DRAW );
}
