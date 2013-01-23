/*
 * shrenderer.cpp
 *
 *  Created on: 03.07.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include <limits>

#include <QtOpenGL/QGLShaderProgram>
#include <QtCore/QDebug>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../../algos/fmath.h"

#include "../../data/enums.h"

#include "../../thirdparty/newmat10/newmat.h"

#include "glfunctions.h"
#include "evrenderer.h"

EVRenderer::EVRenderer( QVector<QVector3D>* data, int nx, int ny, int nz, float dx, float dy, float dz ) :
    ObjectRenderer(),
    m_quads( 0 ),
    vboIds( new GLuint[ 1 ] ),
    m_data( data ),
    m_nx( nx ),
    m_ny( ny ),
    m_nz( nz ),
    m_dx( dx ),
    m_dy( dy ),
    m_dz( dz ),
    m_scaling( 1.0 ),
    m_orient( 0 ),
    m_offset( 0.0 )
{
}

EVRenderer::~EVRenderer()
{
    glDeleteBuffers(1, &( vboIds[ 0 ] ) );
}

void EVRenderer::init()
{
    glGenBuffers( 1, vboIds );
}

void EVRenderer::draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert )
{
    GLFunctions::getShader( "ev" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "ev" )->setUniformValue( "mvp_matrix", mvp_matrix );
    GLFunctions::getShader( "ev" )->setUniformValue( "u_scaling", m_scaling );

    initGeometry();

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    setShaderVars();

    GLfloat lightpos[] = {0.0, 0.0, 1., 0.};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    glDrawArrays( GL_LINES, 0, m_quads );

    GLenum error;
    int i = 0;
    while ((error = glGetError()) != GL_NO_ERROR) {
        qDebug() << "render ev lines: opengl error" << error;
        i++;
    }
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void EVRenderer::setupTextures()
{
}

void EVRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "ev", model() );
}

void EVRenderer::initGeometry()
{
    int xi = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toInt();

    QString s = createSettingsString( xi, yi, zi, m_orient, 0, 0, 0, 0, 0, 0, false, 0, 0, m_offset);

    if ( s == m_previousSettings || m_orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    std::vector<float>verts;

    float x = (float)xi * m_dx + m_dx / 2.;
    float y = (float)yi * m_dy + m_dy / 2.;
    float z = (float)zi * m_dz + m_dz / 2.;

    m_quads = 0;

    if ( m_orient == 1 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int xx = 0; xx < m_nx; ++xx )
            {
                QVector3D vec = m_data->at( xx + yy * m_nx + zi * m_nx * m_ny );

                float locX = xx * m_dx + m_dx / 2;
                float locY = yy * m_dy + m_dy / 2;

                addGlyph( verts, locX, locY, z - m_offset * m_dz , vec );
                m_quads += 2;
            }
        }
    }
    else if ( m_orient == 2 )
    {
        for( int xx = 0; xx < m_nx; ++xx )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                QVector3D vec = m_data->at( xx + yi * m_nx + zz * m_nx * m_ny );

                float locX = xx * m_dx + m_dx / 2;
                float locZ = zz * m_dz + m_dz / 2;

                addGlyph( verts, locX, y + m_offset * m_dy, locZ, vec );
                m_quads += 2;
            }
        }
    }
    else if ( m_orient == 3 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                QVector3D vec = m_data->at( xi + yy * m_nx + zz * m_nx * m_ny );
                float locY = yy * m_dy + m_dy / 2;
                float locZ = zz * m_dz + m_dz / 2;

                addGlyph( verts, x + m_offset * m_dx, locY, locZ, vec );
                m_quads += 2;
            }
        }
    }
    else
    {
        return;
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );
}

void EVRenderer::addGlyph( std::vector<float> &verts, float xPos, float yPos, float zPos, QVector3D vector )
{
    float v0 = vector.x();
    float v1 = vector.y();
    float v2 = vector.z();

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );
}

void EVRenderer::setRenderParams( float scaling, int orient, float offset )
{
    m_scaling = scaling;
    m_orient = orient;
    m_offset = offset;
}