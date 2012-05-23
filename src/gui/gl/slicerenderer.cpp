/*
 * slicerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "../../glew/include/glew.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "glfunctions.h"

#include "slicerenderer.h"

SliceRenderer::SliceRenderer() :
    ObjectRenderer(),
    vboIds( new GLuint[ 4 ] ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_xb( 0. ),
    m_yb( 0. ),
    m_zb( 0. ),
    m_xOld( -1 ),
    m_yOld( -1 ),
    m_zOld( -1 )
{
}

SliceRenderer::~SliceRenderer()
{
    glDeleteBuffers( 4, vboIds );
    delete[] vboIds;
}

void SliceRenderer::init()
{
    initShader();
    glGenBuffers( 4, vboIds );

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    initGeometry();
}

void SliceRenderer::initShader()
{
    m_program = GLFunctions::initShader( "slice" );
}

void SliceRenderer::initGeometry()
{
    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();
    int xi = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toInt();
    m_xb = model()->data( model()->index( 0, 103 ), Qt::UserRole ).toFloat();
    m_yb = model()->data( model()->index( 0, 104 ), Qt::UserRole ).toFloat();
    m_zb = model()->data( model()->index( 0, 105 ), Qt::UserRole ).toFloat();

    float dx = model()->data( model()->index( 0, 106 ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, 107 ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, 108 ), Qt::UserRole ).toFloat();

    m_x *= dx;
    m_y *= dy;
    m_z *= dz;
    m_xb *= dx;
    m_yb *= dy;
    m_zb *= dz;

    if ( zi != m_zOld )
    {
        VertexData verticesAxial[] =
        {
            { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) },
            { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) },
            { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) },
            { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) }
        };
        // Transfer vertex data to VBO 1
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesAxial, GL_STATIC_DRAW );
    }

    if ( yi != m_yOld )
    {
        VertexData verticesCoronal[] =
        {
            { QVector3D( 0.0,  m_y, 0.0  ), QVector3D( 0.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, m_y, 0.0  ), QVector3D( 1.0, m_y/m_yb, 0.0 ) },
            { QVector3D( m_xb, m_y, m_zb ), QVector3D( 1.0, m_y/m_yb, 1.0 ) },
            { QVector3D( 0.0,  m_y, m_zb ), QVector3D( 0.0, m_y/m_yb, 1.0 ) }
        };

        // Transfer vertex data to VBO 2
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCoronal, GL_STATIC_DRAW );
    }

    if ( xi != m_xOld )
    {
        VertexData verticesSagittal[] =
        {
            { QVector3D( m_x, 0.0,  0.0  ), QVector3D( m_x/m_xb, 0.0, 0.0 ) },
            { QVector3D( m_x, m_yb, 0.0  ), QVector3D( m_x/m_xb, 1.0, 0.0 ) },
            { QVector3D( m_x, m_yb, m_zb ), QVector3D( m_x/m_xb, 1.0, 1.0 ) },
            { QVector3D( m_x, 0.0,  m_zb ), QVector3D( m_x/m_xb, 0.0, 1.0 ) }
        };
        // Transfer vertex data to VBO 3
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesSagittal, GL_STATIC_DRAW );
    }

    m_xOld = xi;
    m_yOld = yi;
    m_zOld = zi;
}

void SliceRenderer::setupTextures()
{
    GLFunctions::setupTextures( model() );
}

void SliceRenderer::setShaderVars()
{
    GLFunctions::setSliceShaderVars( m_program, model() );
}

void SliceRenderer::draw( QMatrix4x4 mvp_matrix )
{
    //qDebug() << "main gl draw";
    setupTextures();

    glColor4f( 0.0, 0.0, 0.0, 1.0 );


    // Set modelview-projection matrix
    m_program->setUniformValue( "mvp_matrix", mvp_matrix );

    initGeometry();

    drawAxial();
    drawCoronal();
    drawSagittal();
}

void SliceRenderer::drawAxial()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}

void SliceRenderer::drawCoronal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}

void SliceRenderer::drawSagittal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}
