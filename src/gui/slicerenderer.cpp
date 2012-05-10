/*
 * slicerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "../glew/include/glew.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

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

    VertexData vertices[] =
    {
    // Vertex data for face 0
            { QVector3D( 0.0,   0.0     , 80.0 ), QVector2D( 0.0, 0.0 ) }, // v0
            { QVector3D( 160.0, 0.0     , 80.0 ), QVector2D( 0.33, 0.0 ) }, // v1
            { QVector3D( 160.0, 200.0   , 80.0 ), QVector2D( 0.0, 0.5 ) }, // v2
            { QVector3D( 0.0,   200.0   , 80.0 ), QVector2D( 0.33, 0.5 ) }
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // Transfer vertex data to VBO 0
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

    // Transfer index data to VBO 1
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );
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

    QMatrix4x4 mvMatrix;
    mvMatrix.setToIdentity();
    mvMatrix.translate( -80.0, -100.0, -80.0 );

    mvMatrix = mvMatrix * rotation;
    //mvMatrix.translate( 80.0, 100.0, 80.0 );


    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();

    // Set perspective projection
    pMatrix.ortho( -100, 100, -100, 100, -3000, 3000 );

    // Set modelview-projection matrix
    m_program->setUniformValue( "mvp_matrix", pMatrix * mvMatrix );

    // Draw cube geometry using indices from VBO 1
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

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
