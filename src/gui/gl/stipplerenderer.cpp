/*
 * stipplerenderer.cpp
 *
 *  Created on: May 26, 2014
 *      Author: schurade
 */

#include "stipplerenderer.h"
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

StippleRenderer::StippleRenderer( std::vector<QVector3D>* data ) :
    ObjectRenderer(),
    m_vertCount( 0 ),
    vbo0( 0 ),
    m_data( data ),
    m_mask( 0 ),
    m_scaling( 1.0 ),
    m_orient( 0 ),
    m_offset( 0.0 )
{
    init();
}

StippleRenderer::~StippleRenderer()
{
    glDeleteBuffers( 1, &vbo0 );
    glDeleteBuffers( 1, &vbo1 );
}

void StippleRenderer::setMask( DatasetScalar* mask )
{
    m_mask = mask;
}

void StippleRenderer::init()
{
    glGenBuffers( 1, &vbo0 );
    glGenBuffers( 1, &vbo1 );
}

void StippleRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props )
{
    int slice = 0;
    slice = (int)props.get( Fn::Property::D_RENDER_AXIAL ).toBool() +
            (int)props.get( Fn::Property::D_RENDER_CORONAL ).toBool() * 2 +
            (int)props.get( Fn::Property::D_RENDER_SAGITTAL ).toBool() * 4;
    m_orient = slice;

    if ( ( renderMode != 1 ) )
    {
        return;
    }

    m_scaling = props.get( Fn::Property::D_SCALING ).toFloat();
    m_offset = props.get( Fn::Property::D_OFFSET ).toFloat();
    m_color = props.get( Fn::Property::D_COLOR ).value<QColor>();
    m_lineWidth = props.get( Fn::Property::D_LINE_WIDTH ).toInt();

    QGLShaderProgram* program = GLFunctions::getShader( "stipple" );

    program->bind();
    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "u_scaling", m_scaling );

    program->setUniformValue( "u_alpha", 1.0f );
    program->setUniformValue( "u_renderStipples", true );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    program->setUniformValue( "u_aVec", 1., 0., 0. );
    program->setUniformValue( "u_bVec", 0., 1., 0. );
    program->setUniformValue( "u_glyphThickness", m_lineWidth );

    initGeometry( props );

    setShaderVars( props );
    glLineWidth( m_lineWidth );
    glDrawArrays( GL_TRIANGLES, 0, m_vertCount );
    glLineWidth( 1 );
    GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
}

void StippleRenderer::setShaderVars( PropertyGroup& props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "stipple" );

    program->bind();

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    glBindBuffer( GL_ARRAY_BUFFER, vbo0 );

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );

    offset += sizeof(float) * 3;
    int offsetLocation = program->attributeLocation( "a_vec" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );

    offset += sizeof(float) * 3;
    int dirLocation = program->attributeLocation( "a_dir2" );
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer( dirLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    int colorLocation = program->attributeLocation( "a_color" );
    program->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void StippleRenderer::initGeometry( PropertyGroup& props )
{
    float nx = props.get( Fn::Property::D_NX ).toFloat();
    float ny = props.get( Fn::Property::D_NY ).toFloat();
    float nz = props.get( Fn::Property::D_NZ ).toFloat();

    float dx = props.get( Fn::Property::D_DX ).toFloat();
    float dy = props.get( Fn::Property::D_DY ).toFloat();
    float dz = props.get( Fn::Property::D_DZ ).toFloat();

    float ax = props.get( Fn::Property::D_ADJUST_X ).toFloat();
    float ay = props.get( Fn::Property::D_ADJUST_Y ).toFloat();
    float az = props.get( Fn::Property::D_ADJUST_Z ).toFloat();

    float x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat();
    float y = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat();
    float z = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat();

    int xi = qMax( 0.0f, qMin( ( x + dx / 2 - ax ) / dx, nx - 1 ) );
    int yi = qMax( 0.0f, qMin( ( y + dy / 2 - ay ) / dy, ny - 1 ) );
    int zi = qMax( 0.0f, qMin( ( z + dz / 2 - az ) / dz, nz - 1 ) );


    QString s = createSettingsString( { xi, yi, zi, m_orient, false, m_offset, m_color, m_mask->properties().get( Fn::Property::D_NAME ) } );

    if ( s == m_previousSettings || m_orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    std::vector<float>verts;
    std::vector<float>colors;

    m_vertCount = 0;

    std::vector<float>* probData;
    float max = 0;
    float prob;
    float alpha = 1.0;
    if ( m_mask )
    {
        probData = m_mask->getData();
        //float min = ds2->properties()->get( Fn::Property::D_MIN ).toFloat();
        max = m_mask->properties().get( Fn::Property::D_MAX ).toFloat();
    }
    float randx, randy, randz;

    if ( ( m_orient & 1 ) == 1 )
    {
        for( int yy = 0; yy < ny; ++yy )
        {
            for ( int xx = 0; xx < nx; ++xx )
            {
                QVector3D vec = m_data->at( xx + yy * nx + zi * nx * ny );

                float locX = xx * dx + ax;
                float locY = yy * dy + ay;

                int countStips = 10;
                if ( m_mask )
                {
                    int id = m_mask->getIdFromPos( locX, locY, z - m_offset * dz );
                    prob = probData->at( id );
                    alpha = prob / max;
                    countStips = qMax( 0, (int)( alpha * 10 ) );
                }

                for( int i = 0; i < countStips; ++i )
                {
                    randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * dx - dx;
                    randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * dy - dy;
                    randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * dz - dz;

                    addGlyph( verts, colors,locX + randx, locY + randy, z - m_offset * dz + randz, vec, alpha );
                }
            }
        }
    }
    if ( ( m_orient & 2 ) == 2 )
    {
        for( int xx = 0; xx < nx; ++xx )
        {
            for ( int zz = 0; zz < nz; ++zz )
            {
                QVector3D vec = m_data->at( xx + yi * nx + zz * nx * ny );

                float locX = xx * dx + ax;
                float locZ = zz * dz + az;

                int countStips = 10;
                if ( m_mask )
                {
                    int id = m_mask->getIdFromPos( locX, y + m_offset * dy, locZ );
                    prob = probData->at( id );
                    alpha = prob / max;
                    countStips = qMax( 0, (int)( alpha * 10 ) );
                }

                for( int i = 0; i < countStips; ++i )
                {
                    randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * dx - dx;
                    randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * dy - dy;
                    randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * dz - dz;

                    addGlyph( verts, colors, locX + randx, y + m_offset * dy + randy, locZ + randz, vec, alpha );
                }
            }
        }
    }
    if ( ( m_orient & 4 ) == 4 )
    {
        for( int yy = 0; yy < ny; ++yy )
        {
            for ( int zz = 0; zz < nz; ++zz )
            {
                QVector3D vec = m_data->at( xi + yy * nx + zz * nx * ny );
                float locY = yy * dy + ay;
                float locZ = zz * dz + az;

                int countStips = 10;
                if ( m_mask )
                {
                    int id = m_mask->getIdFromPos( x + m_offset * dx, locY, locZ );
                    prob = probData->at( id );
                    alpha = prob / max;
                    countStips = qMax( 0, (int)( alpha * 10 ) );
                }

                for( int i = 0; i < countStips; ++i )
                {
                    randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * dx - dx;
                    randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * dy - dy;
                    randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * dz - dz;

                    addGlyph( verts, colors, x + m_offset * dx + randx, locY + randy, locZ + randz, vec, alpha );
                }
            }
        }
    }

    glDeleteBuffers( 1, &vbo0 );
    glGenBuffers( 1, &vbo0 );
    glDeleteBuffers( 1, &vbo1 );
    glGenBuffers( 1, &vbo1 );

    glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void StippleRenderer::addGlyph( std::vector<float> &verts, std::vector<float> &colors, float xPos, float yPos, float zPos, QVector3D vector, float alpha )
{
    float v0 = vector.x();
    float v1 = vector.y();
    float v2 = vector.z();

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );
    verts.push_back(  1.0 );
    verts.push_back( -1.0 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );
    verts.push_back( -1.0 );
    verts.push_back(  1.0 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( v0 );
    verts.push_back( v1 );
    verts.push_back( v2 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );


    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( alpha );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( alpha );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( alpha );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( alpha );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( alpha );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( alpha );

    m_vertCount += 6;
}

