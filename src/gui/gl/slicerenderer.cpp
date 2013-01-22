/*
 * slicerenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../../data/enums.h"
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
    m_zOld( -1 ),
    m_xbOld( -1 ),
    m_ybOld( -1 ),
    m_zbOld( -1 )
{
}

SliceRenderer::~SliceRenderer()
{
    glDeleteBuffers( 4, vboIds );
    delete[] vboIds;
}

void SliceRenderer::init()
{
    GLFunctions::loadShaders();
    glGenBuffers( 4, vboIds );

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    initGeometry();
}

void SliceRenderer::initGeometry()
{
    m_x = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toFloat();
    int xi = m_x;
    int yi = m_y;
    int zi = m_z;

    m_xb = model()->data( model()->index( 0, FNGLOBAL_MAX_SAGITTAL ), Qt::UserRole ).toFloat();
    m_yb = model()->data( model()->index( 0, FNGLOBAL_MAX_CORONAL ), Qt::UserRole ).toFloat();
    m_zb = model()->data( model()->index( 0, FNGLOBAL_MAX_AXIAL ), Qt::UserRole ).toFloat();
    int xbi = m_xb;
    int ybi = m_yb;
    int zbi = m_zb;

    float dx = model()->data( model()->index( 0, FNGLOBAL_SLICE_DX ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, FNGLOBAL_SLICE_DY ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, FNGLOBAL_SLICE_DZ ), Qt::UserRole ).toFloat();

    float x = m_x * dx + dx / 2.;
    float y = m_y * dy + dy / 2.;
    float z = m_z * dz + dz / 2.;
    float xb = m_xb * dx;
    float yb = m_yb * dy;
    float zb = m_zb * dz;

    if ( zi != m_zOld || zbi != m_zbOld )
    {
        VertexData verticesAxial[] =
        {
            { QVector3D( 0.0, 0.0, z ), QVector3D( 0.0, 0.0, z / zb ) },
            { QVector3D( xb,  0.0, z ), QVector3D( 1.0, 0.0, z / zb ) },
            { QVector3D( xb,  yb,  z ), QVector3D( 1.0, 1.0, z / zb ) },
            { QVector3D( 0.0, yb,  z ), QVector3D( 0.0, 1.0, z / zb ) }
        };
        // Transfer vertex data to VBO 1
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesAxial, GL_STATIC_DRAW );
    }

    if ( yi != m_yOld || ybi != m_ybOld )
    {
        VertexData verticesCoronal[] =
        {
            { QVector3D( 0.0, y, 0.0 ), QVector3D( 0.0, y / yb, 0.0 ) },
            { QVector3D( xb,  y, 0.0 ), QVector3D( 1.0, y / yb, 0.0 ) },
            { QVector3D( xb,  y, zb  ), QVector3D( 1.0, y / yb, 1.0 ) },
            { QVector3D( 0.0, y, zb  ), QVector3D( 0.0, y / yb, 1.0 ) }
        };

        // Transfer vertex data to VBO 2
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesCoronal, GL_STATIC_DRAW );
    }

    if ( xi != m_xOld  || xbi != m_xbOld )
    {
        VertexData verticesSagittal[] =
        {
            { QVector3D( x, 0.0, 0.0 ), QVector3D( x / xb, 0.0, 0.0 ) },
            { QVector3D( x, yb,  0.0 ), QVector3D( x / xb, 1.0, 0.0 ) },
            { QVector3D( x, yb,  zb  ), QVector3D( x / xb, 1.0, 1.0 ) },
            { QVector3D( x, 0.0, zb  ), QVector3D( x / xb, 0.0, 1.0 ) }
        };
        // Transfer vertex data to VBO 3
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
        glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), verticesSagittal, GL_STATIC_DRAW );
    }

    m_xOld = xi;
    m_yOld = yi;
    m_zOld = zi;
    m_xbOld = xbi;
    m_ybOld = ybi;
    m_zbOld = zbi;
}

void SliceRenderer::setupTextures()
{
    GLFunctions::setupTextures( model() );
}

void SliceRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "slice", model() );
}

void SliceRenderer::draw( QMatrix4x4 mvp_matrix )
{
    //qDebug() << "main gl draw";
    setupTextures();

    glColor4f( 0.0, 0.0, 0.0, 1.0 );

    GLFunctions::getShader( "slice" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "slice" )->setUniformValue( "mvp_matrix", mvp_matrix );

    initGeometry();

    if ( model()->data( model()->index( 0, FNGLOBAL_SHOW_AXIAL ), Qt::UserRole ).toBool() )
    {
        drawAxial();
    }
    if ( model()->data( model()->index( 0, FNGLOBAL_SHOW_CORONAL ), Qt::UserRole ).toBool() )
    {
        drawCoronal();
    }
    if ( model()->data( model()->index( 0, FNGLOBAL_SHOW_SAGITTAL ), Qt::UserRole ).toBool() )
    {
        drawSagittal();
    }
}

void SliceRenderer::drawAxial()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawCoronal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 2 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void SliceRenderer::drawSagittal()
{
    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 3 ] );
    setShaderVars();
    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
