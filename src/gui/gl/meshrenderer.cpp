/*
 * memeshRenderer.cpp
 *
 * Created on: Dec 4, 2012
 * @author Ralph Schurade
 */
#include "meshrenderer.h"

#include "glfunctions.h"

#include "../../data/enums.h"
#include "../../data/mesh/trianglemesh2.h"
#include "../../data/properties/propertygroup.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>
#include <QVector3D>
#include <QMatrix4x4>

#include <limits>

MeshRenderer::MeshRenderer( TriangleMesh2* mesh ) :
    ObjectRenderer(),
    m_tris( 0 ),
    vboIds( new GLuint[ 4 ] ),
    m_pickId( GLFunctions::getPickIndex() ),
    m_mesh( mesh ),
    m_dirty( true ),
    m_colorMode( 0 ),
    m_colormap( 1 ),
    m_selectedMin( 0.0 ),
    m_selectedMax( 1.0 ),
    m_lowerThreshold( 0.0 ),
    m_upperThreshold( 1.0 )
{

}

MeshRenderer::~MeshRenderer()
{
    glDeleteBuffers(1, &( vboIds[ 0 ] ) );
    glDeleteBuffers(1, &( vboIds[ 1 ] ) );
    glDeleteBuffers(1, &( vboIds[ 2 ] ) );
    glDeleteBuffers(1, &( vboIds[ 3 ] ) );
}

void MeshRenderer::setMesh( TriangleMesh2* mesh )
{
    m_mesh = mesh;
    m_dirty = true;
}

void MeshRenderer::init()
{
    glGenBuffers( 4, vboIds );
}

void MeshRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, PropertyGroup* props )
{
    setRenderParams( props );

    GLFunctions::getShader( "mesh" )->bind();

    GLFunctions::setupTextures();
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "mesh" ) );
    // Set modelview-projection matrix
    GLFunctions::getShader( "mesh" )->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    GLFunctions::getShader( "mesh" )->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    GLFunctions::getShader( "mesh" )->setUniformValue( "u_colorMode", m_colorMode );
    GLFunctions::getShader( "mesh" )->setUniformValue( "u_colormap", m_colormap );
    GLFunctions::getShader( "mesh" )->setUniformValue( "u_color", m_color.redF(), m_color.greenF(), m_color.blueF(), 1.0 );
    GLFunctions::getShader( "mesh" )->setUniformValue( "u_selectedMin", m_selectedMin );
    GLFunctions::getShader( "mesh" )->setUniformValue( "u_selectedMax", m_selectedMax );
    GLFunctions::getShader( "mesh" )->setUniformValue( "u_lowerThreshold", m_lowerThreshold );
    GLFunctions::getShader( "mesh" )->setUniformValue( "u_upperThreshold", m_upperThreshold );


    float nx = model()->data( model()->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toFloat();
    float ny = model()->data( model()->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toFloat();
    float nz = model()->data( model()->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toFloat();
    float dx = model()->data( model()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = model()->data( model()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = model()->data( model()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    GLFunctions::getShader( "mesh" )->setUniformValue( "u_dims", nx * dx, ny * dy, nz * dz );

    if ( m_dirty )
    {
        initGeometry();
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();
    glDrawElements( GL_TRIANGLES, m_mesh->numTris()*3, GL_UNSIGNED_INT, 0 );

    glShadeModel( GL_SMOOTH );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void MeshRenderer::setupTextures()
{
}

void MeshRenderer::setShaderVars()
{
    QGLShaderProgram* program = GLFunctions::getShader( "mesh" );

    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int bufferSize = m_mesh->bufferSize();

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * bufferSize, (const void *) offset );
    offset += sizeof(float) * 3;

    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * bufferSize, (const void *) offset );
    offset += sizeof(float) * 3;

    int valueLocation = program->attributeLocation( "a_value" );
    program->enableAttributeArray( valueLocation );
    glVertexAttribPointer( valueLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * bufferSize, (const void *) offset );
    offset += sizeof(float) * 1;

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    if( GLFunctions::isPicking() )
    {
        glShadeModel( GL_FLAT );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    }
    else
    {
        glShadeModel( GL_SMOOTH );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    }
    int colorLocation = program->attributeLocation( "a_color" );
    program->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0 );
}

void MeshRenderer::initGeometry()
{
    int bufferSize = m_mesh->bufferSize();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_mesh->numTris() * 3 * sizeof(GLuint), m_mesh->getIndexes(), GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, m_mesh->numVerts() * bufferSize * sizeof(GLfloat), m_mesh->getVertices(), GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    glBufferData( GL_ARRAY_BUFFER, m_mesh->numVerts() * 4 * sizeof(GLfloat), m_mesh->getVertexColors(), GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    glBufferData( GL_ARRAY_BUFFER, m_mesh->numVerts() * 4 * sizeof(GLfloat), m_mesh->getVertexPickColors(), GL_DYNAMIC_DRAW );

    m_dirty = false;
}

void MeshRenderer::setRenderParams( PropertyGroup* props )
{
    m_colorMode = props->get( Fn::Property::COLORMODE ).toInt();
    m_colormap = props->get( Fn::Property::COLORMAP ).toInt();
    m_selectedMin = props->get( Fn::Property::SELECTED_MIN ).toFloat();
    m_selectedMax = props->get( Fn::Property::SELECTED_MAX ).toFloat();
    m_lowerThreshold = props->get( Fn::Property::LOWER_THRESHOLD ).toFloat();
    m_upperThreshold = props->get( Fn::Property::UPPER_THRESHOLD ).toFloat();
    m_color = props->get( Fn::Property::COLOR ).value<QColor>();
}

void MeshRenderer::beginUpdateColor()
{
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    m_colorBufferPointer = (float*)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
}

void MeshRenderer::endUpdateColor()
{
    glUnmapBuffer( GL_ARRAY_BUFFER );
    m_colorBufferPointer = 0;
}

void MeshRenderer::updateColor( int id, float r, float g, float b, float a )
{
    if ( id != -1 )
    {
        if( m_colorBufferPointer )
        {
            m_colorBufferPointer[ id * 4 ] = r;
            m_colorBufferPointer[ id * 4 + 1] = g;
            m_colorBufferPointer[ id * 4 + 2] = b;
            m_colorBufferPointer[ id * 4 + 3] = a;
        }
    }
}
