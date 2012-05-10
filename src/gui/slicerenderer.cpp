/*
 * slicerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "../glew/include/glew.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>

#include "slicerenderer.h"

struct VertexData
{
    QVector3D position;
    QVector3D texCoord;
};

SliceRenderer::SliceRenderer( DataStore* dataStore ) :
        ObjectRenderer( dataStore ),
        m_program( new QGLShaderProgram ),
        vboIds( new GLuint[ 2 ] )
{
}

SliceRenderer::~SliceRenderer()
{
    glDeleteBuffers( 2, vboIds );
    delete[] vboIds;
}

void SliceRenderer::init()
{
    initShader();
    initGeometry();
}

void SliceRenderer::initShader()
{
    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    // Compiling vertex shader
    if ( !m_program->addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/vshader.glsl" ) )
    {
        exit( false );
    }

    // Compiling fragment shader
    if ( !m_program->addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/fshader.glsl" ) )
    {
        exit( false );
    }

    // Linking shader pipeline
    if ( !m_program->link() )
    {
        exit( false );
    }

    // Binding shader pipeline for use
    if ( !m_program->bind() )
    {
        exit( false );
    }

    // Restore system locale
    setlocale( LC_ALL, "" );
}

void SliceRenderer::initGeometry()
{
    glGenBuffers( 2, vboIds );

    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] =
    {
    // Vertex data for face 0
            { QVector3D( -1.0, -1.0, 1.0 ), QVector2D( 0.0, 0.0 ) }, // v0
            { QVector3D( 1.0, -1.0, 1.0 ), QVector2D( 0.33, 0.0 ) }, // v1
            { QVector3D( -1.0, 1.0, 1.0 ), QVector2D( 0.0, 0.5 ) }, // v2
            { QVector3D( 1.0, 1.0, 1.0 ), QVector2D( 0.33, 0.5 ) }, // v3

            // Vertex data for face 1
            { QVector3D( 1.0, -1.0, 1.0 ), QVector2D( 0.0, 0.5 ) }, // v4
            { QVector3D( 1.0, -1.0, -1.0 ), QVector2D( 0.33, 0.5 ) }, // v5
            { QVector3D( 1.0, 1.0, 1.0 ), QVector2D( 0.0, 1.0 ) }, // v6
            { QVector3D( 1.0, 1.0, -1.0 ), QVector2D( 0.33, 1.0 ) }, // v7

            // Vertex data for face 2
            { QVector3D( 1.0, -1.0, -1.0 ), QVector2D( 0.66, 0.5 ) }, // v8
            { QVector3D( -1.0, -1.0, -1.0 ), QVector2D( 1.0, 0.5 ) }, // v9
            { QVector3D( 1.0, 1.0, -1.0 ), QVector2D( 0.66, 1.0 ) }, // v10
            { QVector3D( -1.0, 1.0, -1.0 ), QVector2D( 1.0, 1.0 ) }, // v11

            // Vertex data for face 3
            { QVector3D( -1.0, -1.0, -1.0 ), QVector2D( 0.66, 0.0 ) }, // v12
            { QVector3D( -1.0, -1.0, 1.0 ), QVector2D( 1.0, 0.0 ) }, // v13
            { QVector3D( -1.0, 1.0, -1.0 ), QVector2D( 0.66, 0.5 ) }, // v14
            { QVector3D( -1.0, 1.0, 1.0 ), QVector2D( 1.0, 0.5 ) }, // v15

            // Vertex data for face 4
            { QVector3D( -1.0, -1.0, -1.0 ), QVector2D( 0.33, 0.0 ) }, // v16
            { QVector3D( 1.0, -1.0, -1.0 ), QVector2D( 0.66, 0.0 ) }, // v17
            { QVector3D( -1.0, -1.0, 1.0 ), QVector2D( 0.33, 0.5 ) }, // v18
            { QVector3D( 1.0, -1.0, 1.0 ), QVector2D( 0.66, 0.5 ) }, // v19

            // Vertex data for face 5
            { QVector3D( -1.0, 1.0, 1.0 ), QVector2D( 0.33, 0.5 ) }, // v20
            { QVector3D( 1.0, 1.0, 1.0 ), QVector2D( 0.66, 0.5 ) }, // v21
            { QVector3D( -1.0, 1.0, -1.0 ), QVector2D( 0.33, 1.0 ) }, // v22
            { QVector3D( 1.0, 1.0, -1.0 ), QVector2D( 0.66, 1.0 ) } // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] =
    { 0, 1, 2, 3, 3, // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
      4, 4, 5, 6, 7, 7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
      8, 8, 9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
      12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
      16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
      20, 20, 21, 22, 23 // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    //! [1]
    // Transfer vertex data to VBO 0
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, 24 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

    // Transfer index data to VBO 1
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 34 * sizeof(GLushort), indices, GL_STATIC_DRAW );
    //! [1]
}

void SliceRenderer::setShaderVars()
{

}

void SliceRenderer::draw( QMatrix4x4 rotation )
{
    m_x = 80.;
    m_y = 100.;
    m_z = 80.;
    m_xb = 160;
    m_yb = 200;
    m_zb = 160;
    m_xc = 1.0;
    m_yc = 1.0;
    m_zc = 1.0;

    glColor4f( 0.0, 0.0, 0.0, 1.0 );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );

    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation( "a_position" );
    m_program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    QMatrix4x4 matrix;
    matrix.translate( 0.0, 0.0, -5.0 );
    matrix = matrix * rotation;

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
    qreal aspect = (qreal) 640 / ( (qreal) 480 ? 480 : 1 );

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective( fov, aspect, zNear, zFar );

    // Set modelview-projection matrix
    m_program->setUniformValue( "mvp_matrix", projection * matrix );

    // Draw cube geometry using indices from VBO 1
    glDrawElements( GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0 );

}

void SliceRenderer::renderAxial()
{
}

void SliceRenderer::renderCoronal()
{
}

void SliceRenderer::renderSagittal()
{
}
