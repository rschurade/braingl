/*
 * orientationhelperrenderer.cpp
 *
 *  Created on: 26.08.2014
 *      Author: Ralph
 */

#include "orientationhelperrenderer.h"

#include "glfunctions.h"

#include "../../data/models.h"

#include <QDebug>
#include <QGLShaderProgram>


OrientationHelperRenderer::OrientationHelperRenderer() :
    vboIds( new GLuint[ 1 ] )
{
}

OrientationHelperRenderer::~OrientationHelperRenderer()
{
    glDeleteBuffers( 1, vboIds );
    delete[] vboIds;
}

void OrientationHelperRenderer::init()
{
    initializeOpenGLFunctions();
    glGenBuffers( 1, vboIds );
    initBox();
}

void OrientationHelperRenderer::initBox()
{
    float x1 = -1.0f;
    float y1 = -1.0f;
    float z1 = -1.0f;
    float x2 = 1.0f;
    float y2 = 1.0f;
    float z2 = 1.0f;

    float vertices[] =
    {
        x1, y1, z1,
        x1, y1, z2,
        x1, y2, z1,
        x1, y2, z2,
        x2, y1, z1,
        x2, y1, z2,
        x2, y2, z1,
        x2, y2, z2,

        x1, y1, z1,
        x2, y1, z1,
        x1, y1, z2,
        x2, y1, z2,
        x1, y2, z1,
        x2, y2, z1,
        x1, y2, z2,
        x2, y2, z2,

        x1, y1, z1,
        x2, y1, z1,
        x1, y2, z1,
        x2, y2, z1,
        x1, y1, z2,
        x2, y1, z2,
        x1, y2, z2,
        x2, y2, z2
    };

    //0        x1, y1, z1,
    //1        x2, y1, z1,
    //2        x2, y2, z1,
    //3        x1, y2, z1,
    //4        x1, y1, z2,
    //5        x2, y1, z2,
    //6        x2, y2, z2,
    //7        x1, y2, z2


    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, 72 * sizeof(float), &vertices, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void OrientationHelperRenderer::drawBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode )
{
    if ( renderMode != 1 )
    {
        return;
    }

    if ( !Models::getGlobal( Fn::Property::G_SHOW_ORIENTHELPER ).toBool() )
    {
        return;
    }

    QGLShaderProgram* program = GLFunctions::getShader( "orienthelper" );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );

    program->bind();
    // Offset for position
    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (const void *) offset );

    // Set modelview-projection matrix

    QMatrix4x4 mv = mv_matrix;
    mv( 0, 3 ) = 0;
    mv( 1, 3 ) = 0;
    mv( 2, 3 ) = 0;

    float zoom = Models::zoom;

    mv.scale( 1.0 / zoom );
    program->setUniformValue( "mvp_matrix", p_matrix * mv );

    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "u_lighting", false );

    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    program->setUniformValue( "u_x", Models::getGlobal( Fn::Property::G_ORIENTHELPER_X ).toFloat() );
    program->setUniformValue( "u_y", Models::getGlobal( Fn::Property::G_ORIENTHELPER_Y ).toFloat() );
    program->setUniformValue( "u_z", Models::getGlobal( Fn::Property::G_ORIENTHELPER_Z ).toFloat() );
    program->setUniformValue( "u_dx", Models::getGlobal( Fn::Property::G_ORIENTHELPER_SIZE ).toFloat() );

    //glEnable(GL_CULL_FACE);
//    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    program->setUniformValue( "u_color", 0.6f, 0.0f, 0.0f, 1.0f );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    program->setUniformValue( "u_color", 1.0f, 0.2f, 0.2f, 1.0f );
    glDrawArrays( GL_TRIANGLE_STRIP, 4, 4 );

    program->setUniformValue( "u_color", 0.0f, 0.7f, 0.0f, 1.0f );
    glDrawArrays( GL_TRIANGLE_STRIP, 8, 4 );
    program->setUniformValue( "u_color", 0.2f, 1.0f, 0.2f, 1.0f );
    glDrawArrays( GL_TRIANGLE_STRIP, 12, 4 );

    program->setUniformValue( "u_color", 0.0f, 0.0f, 0.6f, 1.0f );
    glDrawArrays( GL_TRIANGLE_STRIP, 16, 4 );
    program->setUniformValue( "u_color", 0.2f, 0.2f, 1.0f, 1.0f );
    glDrawArrays( GL_TRIANGLE_STRIP, 20, 4 );

    glDisable(GL_CULL_FACE);

    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "u_lighting", false );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
