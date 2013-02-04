/*
 * boxrenderer.cpp
 *
 *  Created on: 04.02.2013
 *      Author: Ralph
 */
#include "boxrenderer.h"

#include "glfunctions.h"
#include "../../data/enums.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

BoxRenderer::BoxRenderer() :
    ObjectRenderer(),
    vboIds( new GLuint[ 2 ] )
{
}

BoxRenderer::~BoxRenderer()
{
    glDeleteBuffers( 2, vboIds );
    delete[] vboIds;
}

void BoxRenderer::init()
{
    GLFunctions::loadShaders();
    glGenBuffers( 2, vboIds );

    GLushort indices[] = { 3, 2, 1, 0, //bottom
                           0, 1, 5, 4, // front
                           1, 2, 6, 5, // right
                           2, 3, 7, 6, // back
                           3, 0, 4, 7, // left
                           4, 5, 6, 7 // top

                         };
    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    updateGeometry( 0, 0, 0, 0, 0, 0, QColor( 0,0,0 ) );
}

void BoxRenderer::initGeometry()
{

}

void BoxRenderer::updateGeometry( float x, float y, float z, float dx, float dy, float dz, QColor color )
{
    float x1 = x - dx / 2.0;
    float y1 = y - dy / 2.0;
    float z1 = z - dz / 2.0;
    float x2 = x + dx / 2.0;
    float y2 = y + dy / 2.0;
    float z2 = z + dz / 2.0;

    float r = color.redF();
    float g = color.greenF();
    float b = color.blueF();

    VertexData vertices[] =
    {
        { QVector3D( x1, y1, z1 ), QVector3D( r, g, b ) },
        { QVector3D( x2, y1, z1 ), QVector3D( r, g, b ) },
        { QVector3D( x2, y2, z1 ), QVector3D( r, g, b ) },
        { QVector3D( x1, y2, z1 ), QVector3D( r, g, b ) },
        { QVector3D( x1, y1, z2 ), QVector3D( r, g, b ) },
        { QVector3D( x2, y1, z2 ), QVector3D( r, g, b ) },
        { QVector3D( x2, y2, z2 ), QVector3D( r, g, b ) },
        { QVector3D( x1, y2, z2 ), QVector3D( r, g, b ) }
    };

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

}

void BoxRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "box" );
}

void BoxRenderer::draw( QMatrix4x4 mvp_matrix )
{
    GLFunctions::getShader( "box" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "box" )->setUniformValue( "mvp_matrix", mvp_matrix );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_QUADS, 24, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

}
