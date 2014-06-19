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
    m_offset( 0.0 ),
    m_blockSize( 0 ),
    m_nx( 0 ),
    m_ny( 0 ),
    m_nz( 0 ),
    m_dx( 0.0f ),
    m_dy( 0.0f ),
    m_dz( 0.0f ),
    m_ax( 0.0f ),
    m_ay( 0.0f ),
    m_az( 0.0f )
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
    initializeOpenGLFunctions();
    glGenBuffers( 1, &vbo0 );
    glGenBuffers( 1, &vbo1 );
}

void StippleRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props )
{
    if ( ( renderMode == 1 ) )
    {
        return;
    }

    m_orient = props.get( Fn::Property::D_STIPPLE_SLICE_ORIENT ).toInt();
    m_scaling = props.get( Fn::Property::D_SCALING ).toFloat();
    m_offset = props.get( Fn::Property::D_OFFSET ).toFloat();
    m_color = props.get( Fn::Property::D_COLOR ).value<QColor>();
    float thickness = props.get( Fn::Property::D_STIPPLE_THICKNESS ).toFloat();
    float glpyhSize = props.get( Fn::Property::D_STIPPLE_GLYPH_SIZE ).toFloat();

    QGLShaderProgram* program = GLFunctions::getShader( "stipple" );

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

    switch ( m_orient )
    {
        case 0:
            program->setUniformValue( "u_aVec", 1., 0., 0. );
            program->setUniformValue( "u_bVec", 0., 1., 0. );
            break;
        case 1:
            program->setUniformValue( "u_aVec", 1., 0., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            break;
        case 2:
            program->setUniformValue( "u_aVec", 0., 1., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            break;
        default:
            qDebug() << "error, wrong orientation in stipples renderer";
            return;
    }

    program->setUniformValue( "u_orient", m_orient );
    program->setUniformValue( "u_glyphThickness", thickness );
    program->setUniformValue( "u_glyphSize", glpyhSize );

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
    m_nx = props.get( Fn::Property::D_NX ).toFloat();
    m_ny = props.get( Fn::Property::D_NY ).toFloat();
    m_nz = props.get( Fn::Property::D_NZ ).toFloat();

    m_blockSize = m_nx * m_ny * m_nz;

    m_dx = props.get( Fn::Property::D_DX ).toFloat();
    m_dy = props.get( Fn::Property::D_DY ).toFloat();
    m_dz = props.get( Fn::Property::D_DZ ).toFloat();

    m_ax = props.get( Fn::Property::D_ADJUST_X ).toFloat();
    m_ay = props.get( Fn::Property::D_ADJUST_Y ).toFloat();
    m_az = props.get( Fn::Property::D_ADJUST_Z ).toFloat();

    float x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat();
    float y = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat();
    float z = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat();

    int xi = qMax( 0.0f, qMin( ( x + m_dx / 2 - m_ax ) / m_dx, (float)m_nx - 1 ) );
    int yi = qMax( 0.0f, qMin( ( y + m_dy / 2 - m_ay ) / m_dy, (float)m_ny - 1 ) );
    int zi = qMax( 0.0f, qMin( ( z + m_dz / 2 - m_az ) / m_dz, (float)m_nz - 1 ) );


    QString s = createSettingsString( { xi, yi, zi, m_orient, false, m_offset, m_color, m_mask->properties().get( Fn::Property::D_NAME ) } );

    if ( s == m_previousSettings )
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

    if ( m_orient == 0 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int xx = 0; xx < m_nx; ++xx )
            {
                QVector3D vec = m_data->at( xx + yy * m_nx + zi * m_nx * m_ny );

                float locX = xx * m_dx + m_ax;
                float locY = yy * m_dy + m_ay;

                int countStips = 10;
                if ( m_mask )
                {
                    int id = m_mask->getIdFromPos( locX, locY, z - m_offset * m_dz );
                    prob = probData->at( id );
                    alpha = prob / max;
                    countStips = qMax( 0, (int)( alpha * 10 ) );
                }

                for( int i = 0; i < countStips; ++i )
                {
                    randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dx - m_dx;
                    randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dy - m_dy;
                    randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dz - m_dz;

                    addGlyph( verts, colors,locX + randx, locY + randy, z - m_offset * m_dz + randz, vec, alpha );
                }
            }
        }
    }
    if ( m_orient == 1 )
    {
        for( int xx = 0; xx < m_nx; ++xx )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                QVector3D vec = m_data->at( xx + yi * m_nx + zz * m_nx * m_ny );

                float locX = xx * m_dx + m_ax;
                float locZ = zz * m_dz + m_az;

                int countStips = 10;
                if ( m_mask )
                {
                    int id = m_mask->getIdFromPos( locX, y + m_offset * m_dy, locZ );
                    prob = probData->at( id );
                    alpha = prob / max;
                    countStips = qMax( 0, (int)( alpha * 10 ) );
                }

                for( int i = 0; i < countStips; ++i )
                {
                    randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dx - m_dx;
                    randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dy - m_dy;
                    randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dz - m_dz;

                    addGlyph( verts, colors, locX + randx, y + m_offset * m_dy + randy, locZ + randz, vec, alpha );
                }
            }
        }
    }
    if ( m_orient == 2 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                QVector3D vec = m_data->at( xi + yy * m_nx + zz * m_nx * m_ny );
                float locY = yy * m_dy + m_ay;
                float locZ = zz * m_dz + m_az;

                int countStips = 10;
                if ( m_mask )
                {
                    int id = m_mask->getIdFromPos( x + m_offset * m_dx, locY, locZ );
                    prob = probData->at( id );
                    alpha = prob / max;
                    countStips = qMax( 0, (int)( alpha * 10 ) );
                }

                for( int i = 0; i < countStips; ++i )
                {
                    randx = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dx - m_dx;
                    randy = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dy - m_dy;
                    randz = ( (float) rand() / ( RAND_MAX ) ) * 2 * m_dz - m_dz;

                    addGlyph( verts, colors, x + m_offset * m_dx + randx, locY + randy, locZ + randz, vec, alpha );
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
    QVector3D vec = getInterpolatedVector( xPos, yPos, zPos );
    float v0 = vec.x();
    float v1 = vec.y();
    float v2 = vec.z();

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

QVector3D StippleRenderer::getInterpolatedVector( float inx, float iny, float inz )
{
    float x = qMax( 0.0f, qMin( ( inx + m_dx / 2 - m_ax ) / m_dx, (float)m_nx - 1 ) );
    float y = qMax( 0.0f, qMin( ( iny + m_dy / 2 - m_ay ) / m_dy, (float)m_ny - 1 ) );
    float z = qMax( 0.0f, qMin( ( inz + m_dz / 2 - m_az ) / m_dz, (float)m_nz - 1 ) );

    int x0 = (int) x;
    int y0 = (int) y;
    int z0 = (int) z;

    float xd = x - x0;
    float yd = y - y0;
    float zd = z - z0;

    int id = x0 + y0 * m_nx + z0 * m_nx * m_ny;

    int id_x0y0z0 = id;
    int id_x1y0z0 = qMin( m_blockSize - 1, id + 1 );
    int id_x0y1z0 = qMin( m_blockSize - 1, id + m_nx );
    int id_x1y1z0 = qMin( m_blockSize - 1, id + m_nx + 1 );
    int id_x0y0z1 = qMin( m_blockSize - 1, id + m_nx * m_ny );
    int id_x1y0z1 = qMin( m_blockSize - 1, id + m_nx * m_ny + 1 );
    int id_x0y1z1 = qMin( m_blockSize - 1, id + m_nx * m_ny + m_nx );
    int id_x1y1z1 = qMin( m_blockSize - 1, id + m_nx * m_ny + m_nx + 1 );

    QVector3D i1;
    QVector3D i2;
    QVector3D j1;
    QVector3D j2;
    QVector3D w1;
    QVector3D w2;
    QVector3D iv;

    i1 = m_data->at( id_x0y0z0 ) * ( 1.0 - zd ) + m_data->at( id_x0y0z1 ) * zd;
    i2 = m_data->at( id_x0y1z0 ) * ( 1.0 - zd ) + m_data->at( id_x0y1z1 ) * zd;
    j1 = m_data->at( id_x1y0z0 ) * ( 1.0 - zd ) + m_data->at( id_x1y0z1 ) * zd;
    j2 = m_data->at( id_x1y1z0 ) * ( 1.0 - zd ) + m_data->at( id_x1y1z1 ) * zd;

    w1 = i1 * ( 1.0 - yd ) + i2 * yd;
    w2 = j1 * ( 1.0 - yd ) + j2 * yd;

    iv = w1 * ( 1.0 - xd ) + w2 * xd;

    return iv;
}
