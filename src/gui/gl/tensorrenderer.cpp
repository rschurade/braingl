/*
 * shrenderer.cpp
 *
 * Created on: 03.07.2012
 * @author Ralph Schurade
 */
#include "tensorrenderer.h"
#include "glfunctions.h"

#include "../../algos/fmath.h"
#include "../../data/enums.h"
#include "../../data/properties/propertygroup.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>
#include <QVector3D>
#include <QMatrix4x4>

#include <limits>

TensorRenderer::TensorRenderer( std::vector<Matrix>* data ) :
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
    m_gamma( 0.0 ),
    m_orient( 0 ),
    m_offset( 0.0 )
{
}

TensorRenderer::~TensorRenderer()
{
    glDeleteBuffers(2, &( vboIds[ 0 ] ) );
}

void TensorRenderer::init()
{
    glGenBuffers( 2, vboIds );
}

void TensorRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props )
{
    if ( renderMode != 1 ) // we are drawing opaque objects
    {
        // obviously not opaque
        return;
    }

    setRenderParams( props );

    if ( m_orient == 0 )
    {
        return;
    }

    initGeometry();

    QGLShaderProgram* program = GLFunctions::getShader( "superquadric" );

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

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    program->setUniformValue( "u_scaling", m_scaling );
    program->setUniformValue( "u_faThreshold", m_faThreshold );
    program->setUniformValue( "u_evThreshold", m_evThreshold );
    program->setUniformValue( "u_gamma", m_gamma );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    //GLfloat lightpos[] = {0.0, 0.0, 1., 0.};
    // XXX not in CoreProfile; use shader //glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    // XXX not in Core/deprecated //glDrawArrays( GL_QUADS, 0, m_quads );
    glDrawElements( GL_TRIANGLES, m_quads, GL_UNSIGNED_INT, 0 );  // XXX triangle pairs

    GLenum error;
    int i = 0;
    while ((error = glGetError()) != GL_NO_ERROR) {
        qDebug() << "tensor renderer gl error" << error;
        i++;
    }
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void TensorRenderer::setShaderVars( PropertyGroup& props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "superquadric" );

    program->bind();

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );

    offset += sizeof(float) * 3;
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );

    offset += sizeof(float) * 3;
    int offsetLocation = program->attributeLocation( "a_diag" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );

    offset += sizeof(float) * 3;
    int radiusLocation = program->attributeLocation( "a_offdiag" );
    program->enableAttributeArray( radiusLocation );
    glVertexAttribPointer( radiusLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 12, (const void *) offset );
}

void TensorRenderer::initGeometry( PropertyGroup& props )
{
    float dx = model()->data( model()->index( (int)Fn::Property::G_SLICE_DX, 0 ) ).toFloat();
    float dy = model()->data( model()->index( (int)Fn::Property::G_SLICE_DY, 0 ) ).toFloat();
    float dz = model()->data( model()->index( (int)Fn::Property::G_SLICE_DZ, 0 ) ).toFloat();

    int xi = model()->data( model()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toFloat() * ( dx / m_dx );
    int yi = model()->data( model()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toFloat() * ( dy / m_dy );
    int zi = model()->data( model()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toFloat() * ( dz / m_dz );

    xi = qMax( 0, qMin( xi, m_nx - 1) );
    yi = qMax( 0, qMin( yi, m_ny - 1) );
    zi = qMax( 0, qMin( zi, m_nz - 1) );

    QString s = createSettingsString( { xi, yi, zi, m_orient, m_offset } );

    if ( s == m_previousSettings || m_orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    std::vector<float>verts;

    RowVector v1( 3 );
    ColumnVector v2( 3 );

    ColumnVector newVert( 3 );

    float x = (float)xi * m_dx + m_dx / 2;
    float y = (float)yi * m_dy + m_dy / 2;
    float z = (float)zi * m_dz + m_dz / 2;

    m_quads = 0;

    if ( ( m_orient & 1 ) == 1 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int xx = 0; xx < m_nx; ++xx )
            {
                Matrix tensor = m_data->at( xx + yy * m_nx + zi * m_nx * m_ny ) * 1000;

                float locX = xx * m_dx; // + m_dx / 2;
                float locY = yy * m_dy; // + m_dy / 2;

                addGlyph( verts, locX, locY, z - m_offset * m_dz , tensor );
                m_quads += 36;
            }
        }
    }
    if ( ( m_orient & 2 ) == 2 )
    {
        for( int xx = 0; xx < m_nx; ++xx )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                Matrix tensor = m_data->at( xx + yi * m_nx + zz * m_nx * m_ny ) * 1000;

                float locX = xx * m_dx; // + m_dx / 2;
                float locZ = zz * m_dz; // + m_dz / 2;

                addGlyph( verts, locX, y + m_offset * m_dy, locZ, tensor );
                m_quads += 36;
            }
        }
    }
    if ( ( m_orient & 4 ) == 4 )
    {
        for( int yy = 0; yy < m_ny; ++yy )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                Matrix tensor = m_data->at( xi + yy * m_nx + zz * m_nx * m_ny ) * 1000;
                float locY = yy * m_dy; // + m_dy / 2;
                float locZ = zz * m_dz; // + m_dz / 2;

                addGlyph( verts, x + m_offset * m_dx, locY, locZ, tensor );
                m_quads += 36;
            }
        }
    }

    // XXX should be an GL_ARRAY_BUFFER?
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );
    // XXX
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    std::vector<unsigned int> triIndices;
    // fill index vector with two triangle's indices for every quad
    for ( int qi = 0; qi < m_quads / 4; ++qi )
    {
        unsigned int off = qi * 4;
        triIndices.push_back( off );
        triIndices.push_back( off+1 );
        triIndices.push_back( off+3 );

        triIndices.push_back( off+1 );
        triIndices.push_back( off+2 );
        triIndices.push_back( off+3 );
    }
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, triIndices.size() * sizeof(unsigned int), &triIndices[0], GL_STATIC_DRAW );
}

void TensorRenderer::addGlyph( std::vector<float>& verts, float xPos, float yPos, float zPos, Matrix tensor )
{
    float d0 = tensor( 1, 1 );
    float d1 = tensor( 2, 2 );
    float d2 = tensor( 3, 3 );
    float o0 = tensor( 1, 2 );
    float o1 = tensor( 1, 3 );
    float o2 = tensor( 2, 3 );

    // Front Face
    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    // Back Face
    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    // Top Face
    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    // Bottom Face
    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    // Left Face
    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    // Right Face
    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );

    verts.push_back( xPos );
    verts.push_back( yPos );
    verts.push_back( zPos );
    verts.push_back( 1.0 );
    verts.push_back( -1.0 );
    verts.push_back( 1.0 );
    verts.push_back( d0 );
    verts.push_back( d1 );
    verts.push_back( d2 );
    verts.push_back( o0 );
    verts.push_back( o1 );
    verts.push_back( o2 );
}

void TensorRenderer::setRenderParams( PropertyGroup& props )
{
    int slice = 0;
    slice = (int)props.get( Fn::Property::D_RENDER_AXIAL ).toBool() +
            (int)props.get( Fn::Property::D_RENDER_CORONAL ).toBool() * 2 +
            (int)props.get( Fn::Property::D_RENDER_SAGITTAL ).toBool() * 4;

    m_scaling = props.get( Fn::Property::D_SCALING ).toFloat();
    m_faThreshold = props.get( Fn::Property::D_FA_THRESHOLD ).toFloat();
    m_evThreshold = props.get( Fn::Property::D_EV_THRESHOLD ).toFloat();
    m_gamma = props.get( Fn::Property::D_GAMMA ).toFloat();
    m_orient = slice;
    m_offset = props.get( Fn::Property::D_OFFSET ).toFloat();
}
