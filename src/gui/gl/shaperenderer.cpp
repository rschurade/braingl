/*
 * shaperenderer.cpp
 *
 * Created on: Mar 11, 2013
 * @author schurade
 */

#include "shaperenderer.h"
#include "glfunctions.h"

#include "../../data/mesh/tesselation.h"

#include <QDebug>
#include <QtOpenGL/QGLShaderProgram>

ShapeRenderer::ShapeRenderer() :
    vboIds( new GLuint[ 4 ] )
{
}

ShapeRenderer::~ShapeRenderer()
{
    glDeleteBuffers( 4, vboIds );
    delete[] vboIds;
}

void ShapeRenderer::init()
{
    glGenBuffers( 4, vboIds );
    initBox();
    initSphere();
}

void ShapeRenderer::initBox()
{
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

    float x1 = -1.0f;
    float y1 = -1.0f;
    float z1 = -1.0f;
    float x2 = 1.0f;
    float y2 = 1.0f;
    float z2 = 1.0f;

    float vertices[] =
    {
        x1, y1, z1,
        x2, y1, z1,
        x2, y2, z1,
        x1, y2, z1,
        x1, y1, z2,
        x2, y1, z2,
        x2, y2, z2,
        x1, y2, z2
    };

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices, GL_STATIC_DRAW );

}

void ShapeRenderer::initSphere()
{
    int lod = 3;

    const Matrix* vertices = tess::vertices( lod );
    const int* faces = tess::faces( lod );
    int numVerts = tess::n_vertices( lod );
    int numTris = tess::n_faces( lod );

    std::vector<float>verts;
    verts.reserve( numVerts * 3 );
    std::vector<int>indexes;
    indexes.reserve( numTris * 3 );


    for ( int i = 0; i < numVerts; ++i )
    {
        verts.push_back( (*vertices)( i+1, 1 ) );
        verts.push_back( (*vertices)( i+1, 2 ) );
        verts.push_back( (*vertices)( i+1, 3 ) );
    }
    for ( int i = 0; i < numTris; ++i )
    {
        indexes.push_back( faces[i*3] );
        indexes.push_back( faces[i*3+1] );
        indexes.push_back( faces[i*3+2] );
    }


    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 2 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 3 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );
}

void ShapeRenderer::drawBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                                float x, float y, float z, float dx, float dy, float dz,
                                QColor color, int pickID, int width, int height, int renderMode )
{
    float alpha = color.alphaF();

    if ( renderMode != 1 ) // we are not picking
    {
        if ( renderMode == 4 ) // we are drawing opaque objects
        {
            if ( alpha < 1.0 )
            {
                // obviously not opaque
                return;
            }
        }
        else // we are drawing tranparent objects
        {
            if ( !(alpha < 1.0 ) || alpha == 0.0 )
            {
                // not transparent
                return;
            }
        }
    }

    QGLShaderProgram* program = GLFunctions::getShader( "shape" );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );

    program->bind();
    // Offset for position
    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (const void *) offset );

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    if( renderMode == 1 )
    {
        float alpha =  1.0;
        float blue =  (float)(( pickID ) & 0xFF) / 255.f;
        float green = (float)(( pickID >> 8 ) & 0xFF) / 255.f;
        float red =   (float)(( pickID >> 16 ) & 0xFF) / 255.f;

        //qDebug() << " input" << red << green << blue << alpha ;
        program->setUniformValue( "u_picking", true );
        program->setUniformValue( "u_renderMode", 1 );
        program->setUniformValue( "u_color", red, green , blue, alpha );
        program->setUniformValue( "u_lighting", false );
    }
    else
    {
        program->setUniformValue( "u_picking", false );
        program->setUniformValue( "u_color", color.redF(), color.greenF(), color.blueF(), color.alphaF() );
        program->setUniformValue( "u_alpha", (float)color.alphaF() );
        program->setUniformValue( "u_renderMode", renderMode );
        program->setUniformValue( "u_canvasSize", width, height );
        program->setUniformValue( "u_lighting", false );
    }

    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );

    program->setUniformValue( "u_x", x );
    program->setUniformValue( "u_y", y );
    program->setUniformValue( "u_z", z );
    program->setUniformValue( "u_dx", dx / 2 );
    program->setUniformValue( "u_dy", dy / 2 );
    program->setUniformValue( "u_dz", dz / 2 );

    glEnable(GL_CULL_FACE);
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_QUADS, 24, GL_UNSIGNED_SHORT, 0 );

    glDisable(GL_CULL_FACE);

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void ShapeRenderer::drawSphere( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                                    float x, float y, float z, float dx, float dy, float dz,
                                    QColor color, int pickID, int width, int height, int renderMode )
{
    float alpha = color.alphaF();

    if ( renderMode != 1 ) // we are not picking
    {
        if ( renderMode == 4 ) // we are drawing opaque objects
        {
            if ( alpha < 1.0 )
            {
                // obviously not opaque
                return;
            }
        }
        else // we are drawing tranparent objects
        {
            if ( !(alpha < 1.0 )  || alpha == 0.0 )
            {
                // not transparent
                return;
            }
        }
    }


    QGLShaderProgram* program = GLFunctions::getShader( "shape" );

    program->bind();

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 2 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );


    // Offset for position
    long int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (const void *) offset );

    program->setUniformValue( "u_x", x );
    program->setUniformValue( "u_y", y );
    program->setUniformValue( "u_z", z );
    program->setUniformValue( "u_dx", dx / 2 );
    program->setUniformValue( "u_dy", dy / 2 );
    program->setUniformValue( "u_dz", dz / 2 );
    if( renderMode == 1 )
    {
        float alpha =  1.0;
        float blue =  (float)(( pickID ) & 0xFF) / 255.f;
        float green = (float)(( pickID >> 8 ) & 0xFF) / 255.f;
        float red =   (float)(( pickID >> 16 ) & 0xFF) / 255.f;

        //qDebug() << " input" << red << green << blue << alpha ;
        program->setUniformValue( "u_color", red, green , blue, alpha );
        program->setUniformValue( "u_picking", true );
        program->setUniformValue( "u_renderMode", 1 );
        program->setUniformValue( "u_lighting", false );
    }
    else
    {
        program->setUniformValue( "u_picking", false );
        program->setUniformValue( "u_color", color.redF(), color.greenF(), color.blueF(), color.alphaF() );
        program->setUniformValue( "u_alpha", (float)color.alphaF() );
        program->setUniformValue( "u_renderMode", renderMode );
        program->setUniformValue( "u_canvasSize", width, height );
        program->setUniformValue( "u_lighting", true );
    }

    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );

    glEnable(GL_CULL_FACE);
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    glDrawElements( GL_TRIANGLES, tess::n_faces( 3 ) * 3, GL_UNSIGNED_INT, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glDisable(GL_CULL_FACE);
}


