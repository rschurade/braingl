/*
 * sphererenderer.cpp
 *
 *  Created on: 07.03.2013
 *      Author: Ralph Schurade
 */
#include "sphererenderer.h"

#include "glfunctions.h"
#include "../../data/enums.h"
#include "../../data/mesh/tesselation.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

SphereRenderer::SphereRenderer() :
    ObjectRenderer(),
    vboIds( new GLuint[ 2 ] ),
    m_pickId( GLFunctions::getPickIndex() )
{
}

SphereRenderer::~SphereRenderer()
{
    glDeleteBuffers( 2, vboIds );
    delete[] vboIds;
}

unsigned int SphereRenderer::getPickId()
{
    return m_pickId;
}

void SphereRenderer::init()
{
    GLFunctions::loadShaders();
    glGenBuffers( 2, vboIds );



    initGeometry();
}

void SphereRenderer::initGeometry()
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


    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );
}

void SphereRenderer::updateGeometry( float x, float y, float z, float dx, float dy, float dz, QColor color )
{
    m_x = x;
    m_y = y;
    m_z = z;
    m_dx = dx;
    m_dy = dy;
    m_dz = dz;
    m_color = color;
}

void SphereRenderer::setShaderVars()
{
    QGLShaderProgram* program = GLFunctions::getShader( "sphere" );

    program->bind();
    // Offset for position
    long int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (const void *) offset );

    GLFunctions::getShader( "sphere" )->setUniformValue( "u_x", m_x );
    GLFunctions::getShader( "sphere" )->setUniformValue( "u_y", m_y );
    GLFunctions::getShader( "sphere" )->setUniformValue( "u_z", m_z );
    GLFunctions::getShader( "sphere" )->setUniformValue( "u_dx", m_dx / 2 );
    GLFunctions::getShader( "sphere" )->setUniformValue( "u_dy", m_dy / 2 );
    GLFunctions::getShader( "sphere" )->setUniformValue( "u_dz", m_dz / 2 );
    if( GLFunctions::isPicking() )
    {
        float alpha =  1.0;
        float blue =  (float)(( m_pickId ) & 0xFF) / 255.f;
        float green = (float)(( m_pickId >> 8 ) & 0xFF) / 255.f;
        float red =   (float)(( m_pickId >> 16 ) & 0xFF) / 255.f;

        //qDebug() << " input" << red << green << blue << alpha ;
        GLFunctions::getShader( "sphere" )->setUniformValue( "u_color", red, green , blue, alpha );
        GLFunctions::getShader( "sphere" )->setUniformValue( "u_picking", true );
    }
    else
    {
        GLFunctions::getShader( "sphere" )->setUniformValue( "u_color", m_color.redF(), m_color.greenF(), m_color.blueF(), 1.0 );
        GLFunctions::getShader( "sphere" )->setUniformValue( "u_picking", false );
    }
}

void SphereRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix )
{
    GLFunctions::getShader( "sphere" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "sphere" )->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    GLFunctions::getShader( "sphere" )->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();
    glDrawElements( GL_TRIANGLES, tess::n_faces( 3 ) * 3, GL_UNSIGNED_INT, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

}
