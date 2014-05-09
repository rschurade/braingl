/*
 * shrenderer.cpp
 *
 * Created on: 03.07.2012
 * @author Ralph Schurade
 */
#include "evrenderer.h"
#include "glfunctions.h"

#include "../../algos/fmath.h"
#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/properties/propertygroup.h"
#include "../../data/datasets/datasetscalar.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>
#include <QVector3D>
#include <QMatrix4x4>

#include <limits>

EVRenderer::EVRenderer( std::vector<QVector3D>* data, int nx, int ny, int nz, float dx, float dy, float dz ) :
    ObjectRenderer(),
    m_vertCount( 0 ),
    vbo( 0 ),
    m_data( data ),
    m_mask( 0 ),
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
    init();
}

EVRenderer::~EVRenderer()
{
    glDeleteBuffers( 1, &vbo );
}

void EVRenderer::setMask( DatasetScalar* mask )
{
    m_mask = mask;
}

void EVRenderer::init()
{
    glGenBuffers( 1, &vbo );
}

void EVRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props )
{
    int slice = 0;
    slice = (int)props->get( Fn::Property::D_RENDER_AXIAL ).toBool() +
            (int)props->get( Fn::Property::D_RENDER_CORONAL ).toBool() * 2 +
            (int)props->get( Fn::Property::D_RENDER_SAGITTAL ).toBool() * 4;

     m_orient = slice;

    if ( ( renderMode != 1 ) || ( renderMode == 0 ) ||  ( m_orient == 0 ) ) // we are drawing opaque objects
    {
        // obviously not opaque
        return;
    }

    m_scaling = props->get( Fn::Property::D_SCALING ).toFloat();
    m_offset = props->get( Fn::Property::D_OFFSET ).toFloat();
    m_renderStipples = props->get( Fn::Property::D_RENDER_VECTORS_STIPPLES ).toBool();

    QGLShaderProgram* program = GLFunctions::getShader( "ev" );

    program->bind();
    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "u_scaling", m_scaling );

    program->setUniformValue( "u_alpha", 1.0f );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    initGeometry();

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    setShaderVars();

    glDrawArrays( GL_LINES, 0, m_vertCount );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    GLFunctions::getAndPrintGLError( "render ev lines: opengl error" );
}

void EVRenderer::setupTextures()
{
}

void EVRenderer::setShaderVars()
{
    QGLShaderProgram* program = GLFunctions::getShader( "ev" );

    program->bind();

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float) * 3;
    int dirLocation = program->attributeLocation( "a_dir" );
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer( dirLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float) * 1;
    int offsetLocation = program->attributeLocation( "a_vec" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );
}

void EVRenderer::initGeometry()
{
    Models::getGlobal( Fn::Property::G_SLICE_DX ).toFloat();
    float dx = Models::getGlobal( Fn::Property::G_SLICE_DX ).toFloat();
    float dy = Models::getGlobal( Fn::Property::G_SLICE_DY ).toFloat();
    float dz = Models::getGlobal( Fn::Property::G_SLICE_DZ ).toFloat();

    int xi = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat() * ( dx / m_dx );
    int yi = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat() * ( dy / m_dy );
    int zi = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat() * ( dz / m_dz );

    xi = qMax( 0, qMin( xi, m_nx - 1) );
    yi = qMax( 0, qMin( yi, m_ny - 1) );
    zi = qMax( 0, qMin( zi, m_nz - 1) );

    QString s = createSettingsString( { xi, yi, zi, m_orient, false, m_offset, m_renderStipples } );

    if ( s == m_previousSettings || m_orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    std::vector<float>verts;

    float x = (float)xi * m_dx + m_dx / 2.;
    float y = (float)yi * m_dy + m_dy / 2.;
    float z = (float)zi * m_dz + m_dz / 2.;

    m_vertCount = 0;

    std::vector<float>* probData;
    float max;
    if ( m_mask )
    {
        probData = m_mask->getData();
        //float min = ds2->properties()->get( Fn::Property::D_MIN ).toFloat();
        max = m_mask->properties()->get( Fn::Property::D_MAX ).toFloat();
    }
    float randx, randy, randz;

    if ( ( m_orient & 1 ) == 1 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int xx = 0; xx < m_nx; ++xx )
            {
                QVector3D vec = m_data->at( xx + yy * m_nx + zi * m_nx * m_ny );

                float locX = xx * m_dx + m_dx / 2;
                float locY = yy * m_dy + m_dy / 2;

                if( m_renderStipples )
                {
                    int countStips = 10;
                    if ( m_mask )
                    {
                        int id = m_mask->getIdFromPos( locX, locY, z - m_offset * m_dz );
                        float prob = probData->at( id );
                        countStips = qMax( 0, (int)( ( prob / max ) * 10 ) );
                    }

                    for( int i = 0; i < countStips; ++i )
                    {
                        randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * dx - dx;
                        randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * dy - dy;
                        randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * dz - dz;

                        addGlyph( verts, locX + randx, locY + randy, z - m_offset * m_dz + randz, vec );
                        m_vertCount += 2;
                    }
                }
                else
                {
                    addGlyph( verts, locX, locY, z - m_offset * m_dz , vec );
                    m_vertCount += 2;
                }
            }
        }
    }
    if ( ( m_orient & 2 ) == 2 )
    {
        for( int xx = 0; xx < m_nx; ++xx )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                QVector3D vec = m_data->at( xx + yi * m_nx + zz * m_nx * m_ny );

                float locX = xx * m_dx + m_dx / 2;
                float locZ = zz * m_dz + m_dz / 2;

                if( m_renderStipples )
                {
                    int countStips = 10;
                    if ( m_mask )
                    {
                        int id = m_mask->getIdFromPos( locX, y + m_offset * m_dy, locZ );
                        float prob = probData->at( id );
                        countStips = qMax( 0, (int)( ( prob / max ) * 10 ) );
                    }

                    for( int i = 0; i < countStips; ++i )
                    {
                        randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * dx - dx;
                        randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * dy - dy;
                        randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * dz - dz;

                        addGlyph( verts, locX + randx, y + m_offset * m_dy + randy, locZ + randz, vec );
                        m_vertCount += 2;
                    }
                }
                else
                {
                    addGlyph( verts, locX, y + m_offset * m_dy, locZ, vec );
                    m_vertCount += 2;
                }
            }
        }
    }
    if ( ( m_orient & 4 ) == 4 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                QVector3D vec = m_data->at( xi + yy * m_nx + zz * m_nx * m_ny );
                float locY = yy * m_dy + m_dy / 2;
                float locZ = zz * m_dz + m_dz / 2;

                if( m_renderStipples )
                {
                    int countStips = 10;
                    if ( m_mask )
                    {
                        int id = m_mask->getIdFromPos( x + m_offset * m_dx, locY, locZ );
                        float prob = probData->at( id );
                        countStips = qMax( 0, (int)( ( prob / max ) * 10 ) );
                    }

                    for( int i = 0; i < countStips; ++i )
                    {
                        randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * dx - dx;
                        randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * dy - dy;
                        randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * dz - dz;

                        addGlyph( verts, x + m_offset * m_dx + randx, locY + randy, locZ + randz, vec );
                        m_vertCount += 2;
                    }
                }
                else
                {
                    addGlyph( verts, x + m_offset * m_dx, locY, locZ, vec );
                    m_vertCount += 2;
                }
            }
        }
    }

    glDeleteBuffers( 1, &vbo );
    glGenBuffers( 1, &vbo );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
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

