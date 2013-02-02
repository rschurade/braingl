/*
 * shrenderer.cpp
 *
 *  Created on: 03.07.2012
 *      Author: Ralph
 */
#include "tensorrendererev.h"
#include "glfunctions.h"

#include "../../algos/fmath.h"
#include "../../data/enums.h"
#include "../../thirdparty/newmat10/newmat.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QtCore/QDebug>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include <limits>

TensorRendererEV::TensorRendererEV( QVector<Matrix>* data, int nx, int ny, int nz, float dx, float dy, float dz ) :
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
    m_faThreshold( 0.0 ),
    m_evThreshold( 0.0 ),
    m_orient( 0 ),
    m_offset( 0.0 ),
    m_evSelect( 1 )
{
}

TensorRendererEV::~TensorRendererEV()
{
    glDeleteBuffers(1, &( vboIds[ 0 ] ) );
}

void TensorRendererEV::init()
{
    glGenBuffers( 1, vboIds );
}

void TensorRendererEV::draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert )
{
    GLFunctions::getShader( "tensorev" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "tensorev" )->setUniformValue( "mvp_matrix", mvp_matrix );

    GLFunctions::getShader( "tensorev" )->setUniformValue( "u_scaling", m_scaling );
    GLFunctions::getShader( "tensorev" )->setUniformValue( "u_faThreshold", m_faThreshold );
    GLFunctions::getShader( "tensorev" )->setUniformValue( "u_evThreshold", m_evThreshold );
    GLFunctions::getShader( "tensorev" )->setUniformValue( "u_evSelect", m_evSelect );

    initGeometry();

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    setShaderVars();

    GLfloat lightpos[] = {0.0, 0.0, 1., 0.};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    glDrawArrays( GL_LINES, 0, m_quads );

    GLenum error;
    int i = 0;
    while ((error = glGetError()) != GL_NO_ERROR) {
        qDebug() << "render tensor lines: opengl error" << error;
        i++;
    }
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void TensorRendererEV::setupTextures()
{
}

void TensorRendererEV::setShaderVars()
{
    GLFunctions::setShaderVars( "tensorev", model() );
}

void TensorRendererEV::initGeometry()
{
    int xi = model()->data( model()->index( FNGLOBAL_SAGITTAL, 0 ) ).toInt();
    int yi = model()->data( model()->index( FNGLOBAL_CORONAL, 0 ) ).toInt();
    int zi = model()->data( model()->index( FNGLOBAL_AXIAL, 0 ) ).toInt();

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
                Matrix tensor = m_data->at( xx + yy * m_nx + zi * m_nx * m_ny ) * 1000;

                float locX = xx * m_dx + m_dx / 2;
                float locY = yy * m_dy + m_dy / 2;

                addGlyph( &verts, locX, locY, z - m_offset * m_dz , tensor );
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
                Matrix tensor = m_data->at( xx + yi * m_nx + zz * m_nx * m_ny ) * 1000;

                float locX = xx * m_dx + m_dx / 2;
                float locZ = zz * m_dz + m_dz / 2;

                addGlyph( &verts, locX, y + m_offset * m_dy, locZ, tensor );
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
                Matrix tensor = m_data->at( xi + yy * m_nx + zz * m_nx * m_ny ) * 1000;
                float locY = yy * m_dy + m_dy / 2;
                float locZ = zz * m_dz + m_dz / 2;

                addGlyph( &verts, x + m_offset * m_dx, locY, locZ, tensor );
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

void TensorRendererEV::addGlyph( std::vector<float>* verts, float xPos, float yPos, float zPos, Matrix tensor )
{
    float d0 = tensor( 1, 1 );
    float d1 = tensor( 2, 2 );
    float d2 = tensor( 3, 3 );
    float o0 = tensor( 1, 2 );
    float o1 = tensor( 1, 3 );
    float o2 = tensor( 2, 3 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );
}

void TensorRendererEV::setRenderParams( float scaling, float faThreshold, float evThreshold, int orient, float offset, int evSelect )
{
    m_scaling = scaling;
    m_faThreshold = faThreshold;
    m_evThreshold = evThreshold;
    m_orient = orient;
    m_offset = offset;
    m_evSelect = evSelect;
}
