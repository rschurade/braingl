/*
 * datasetisoline.cpp
 *
 *  Created on: 08.05.2014
 *      Author: Ralph
 */

#include "datasetisoline.h"

#include "datasetscalar.h"

#include "../enums.h"
#include "../models.h"

#include "../../algos/marchingsquares.h"

#include "../../gui/gl/glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>

DatasetIsoline::DatasetIsoline( DatasetScalar* ds )  :
    Dataset( QDir( "new iso line" ), Fn::DatasetType::ISO_LINE ),
    m_dirty( true ),
    vbo0( 0 ),
    vbo1( 0 ),
    m_countLines( 0 )
{
    m_scalarField = *(ds->getData() );

    m_properties["maingl"].createInt( Fn::Property::D_NX, ds->properties( "maingl" ).get( Fn::Property::D_NX ).toInt() );
    m_properties["maingl"].createInt( Fn::Property::D_NY, ds->properties( "maingl" ).get( Fn::Property::D_NY ).toInt() );
    m_properties["maingl"].createInt( Fn::Property::D_NZ, ds->properties( "maingl" ).get( Fn::Property::D_NZ ).toInt() );
    m_properties["maingl"].createFloat( Fn::Property::D_DX, ds->properties( "maingl" ).get( Fn::Property::D_DX ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_DY, ds->properties( "maingl" ).get( Fn::Property::D_DY ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_DZ, ds->properties( "maingl" ).get( Fn::Property::D_DZ ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_X, ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_X ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_Y, ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_Y ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_Z, ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_Z ).toFloat() );

    m_properties["maingl"].createInt( Fn::Property::D_DIM, 0 );
    m_properties["maingl"].createInt( Fn::Property::D_CREATED_BY, (int)Fn::Algo::ISOSURFACE );
    m_properties["maingl"].createInt( Fn::Property::D_TYPE, (int)Fn::DatasetType::ISO_LINE );

    QString name = ds->properties( "maingl" ).get( Fn::Property::D_NAME ).toString() + " (isoline)";

    m_properties["maingl"].createString( Fn::Property::D_NAME, name );

    m_properties["maingl"].createBool( Fn::Property::D_RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_CORONAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_AXIAL, true, "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_SAGITTAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_CORONAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_AXIAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );
    m_properties["maingl"].createInt( Fn::Property::D_LINE_WIDTH, 1, 1, 10, "general" );
    //connect( m_properties["maingl"].getProperty( Fn::Property::D_LINE_WIDTH ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );
    m_properties["maingl"].createColor( Fn::Property::D_COLOR, Models::getGlobal( Fn::Property::G_ISOLINE_STANDARD_COLOR ).value<QColor>(), "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    m_properties["maingl"].createBool( Fn::Property::D_INTERPOLATION, true, "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_INTERPOLATION ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    m_properties["maingl"].createFloat( Fn::Property::D_ISO_VALUE, 0.0f, ds->properties( "maingl" ).get( Fn::Property::D_MIN ).toFloat(), ds->properties( "maingl" ).get( Fn::Property::D_MAX ).toFloat(), "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_ISO_VALUE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );

    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( globalChanged() ) );
}

DatasetIsoline::~DatasetIsoline()
{
}

void DatasetIsoline::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    if ( m_dirty )
    {
        initGeometry();
    }

    if ( m_countLines == 0 )
    {
        return;
    }

    float alpha = GLFunctions::sliceAlpha[target];

    switch ( renderMode )
    {
        case 0:
            break;
        case 1:
        {
            if ( alpha < 1.0 ) // obviously not opaque
            {
                return;
            }
            break;
        }
        default:
        {
            if ( alpha == 1.0  ) // not transparent
            {
                return;
            }
            break;
        }
    }

    GLFunctions::getShader( "line" )->bind();
    GLFunctions::getShader( "line" )->setUniformValue( "mvp_matrix", pMatrix * mvMatrix );
    GLFunctions::getShader( "line" )->setUniformValue( "u_renderStipples", false );

    glLineWidth(  m_properties["maingl"].get( Fn::Property::D_LINE_WIDTH ).toInt() );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
    // Draw cube geometry using indices from VBO 0
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = GLFunctions::getShader( "line" )->attributeLocation( "a_position" );
    GLFunctions::getShader( "line" )->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    // Draw cube geometry using indices from VBO 1
    int colorLocation = GLFunctions::getShader( "line" )->attributeLocation( "a_color" );
    GLFunctions::getShader( "line" )->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0 );


    // Draw cube geometry using indices from VBO 0
    glDrawArrays( GL_LINES, 0 , m_countLines );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glLineWidth(  1 );
}

void DatasetIsoline::initGeometry()
{
    if( vbo0 )
    {
        glDeleteBuffers( 1, &vbo0 );
    }
    glGenBuffers( 1, &vbo0 );
    if( vbo1 )
    {
        glDeleteBuffers( 1, &vbo1 );
    }
    glGenBuffers( 1, &vbo1 );

    std::vector<float>sliceData;
    std::vector<float>isoVerts;
    std::vector<float>tmpVerts;

    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();
    float dx = m_properties["maingl"].get( Fn::Property::D_DX ).toFloat();
    float dy = m_properties["maingl"].get( Fn::Property::D_DY ).toFloat();
    float dz = m_properties["maingl"].get( Fn::Property::D_DZ ).toFloat();
    float px = m_properties["maingl"].get( Fn::Property::D_ADJUST_X ).toFloat();
    float py = m_properties["maingl"].get( Fn::Property::D_ADJUST_Y ).toFloat();
    float pz = m_properties["maingl"].get( Fn::Property::D_ADJUST_Z ).toFloat();
    m_x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat();
    m_y = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat();
    m_z = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat();

    float isoValue = m_properties["maingl"].get( Fn::Property::D_ISO_VALUE ).toFloat();
    bool interpolation = m_properties["maingl"].get( Fn::Property::D_INTERPOLATION ).toBool();

    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_SAGITTAL ).toBool() )
    {
        sliceData = extractAnatomySagittal( m_x / dx );
        MarchingSquares ms1( &sliceData, isoValue, ny, nz, dy, dz, interpolation );
        tmpVerts = ms1.run();

        for ( unsigned int i = 0; i < tmpVerts.size() / 3; ++i )
        {
            isoVerts.push_back( m_x + px );
            isoVerts.push_back( tmpVerts[3*i] + py );
            isoVerts.push_back( tmpVerts[3*i+1] + pz );
        }
    }
    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_CORONAL ).toBool() )
    {
        sliceData = extractAnatomyCoronal( m_y / dy );
        MarchingSquares ms1( &sliceData, isoValue, nx, nz, dx, dz, interpolation );
        tmpVerts = ms1.run();

        for ( unsigned int i = 0; i < tmpVerts.size() / 3; ++i )
        {
            isoVerts.push_back( tmpVerts[3*i] + px );
            isoVerts.push_back( m_y + py );
            isoVerts.push_back( tmpVerts[3*i+1] + pz );
        }
    }
    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_AXIAL ).toBool() )
    {
        sliceData = extractAnatomyAxial( m_z / dz );
        MarchingSquares ms1( &sliceData, isoValue, nx, ny, dx, dy, interpolation );
        tmpVerts = ms1.run();

        for ( unsigned int i = 0; i < tmpVerts.size() / 3; ++i )
        {
            isoVerts.push_back( tmpVerts[3*i] + px );
            isoVerts.push_back( tmpVerts[3*i+1] + py );
            isoVerts.push_back( m_z + pz );
        }
    }

    glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
    glBufferData( GL_ARRAY_BUFFER, isoVerts.size() * sizeof( float ), isoVerts.data(), GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    std::vector<float>colors( ( isoVerts.size() / 3 ) * 4, 0.0 );
    QColor col = m_properties["maingl"].get( Fn::Property::D_COLOR ).value<QColor>();
    for ( unsigned int i = 0; i < colors.size() / 4; ++i )
    {
        colors[i * 4    ] = col.redF();
        colors[i * 4 + 1] = col.greenF();
        colors[i * 4 + 2] = col.blueF();
        colors[i * 4 + 3] = 1.0;
    }

    glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
    glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof( float ), colors.data(), GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    m_countLines = isoVerts.size() / 2;
    m_dirty = false;
}

std::vector<float> DatasetIsoline::extractAnatomyAxial( float z )
{
    std::vector<float>sliceData;
    int nx =  m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny =  m_properties["maingl"].get( Fn::Property::D_NY ).toInt();

    sliceData.resize( nx * ny, 0 );
    for ( int y = 0; y < ny; ++y )
    {
        for ( int x = 0; x < nx; ++x )
        {
            int id = getId( x, y, z );
            float value = m_scalarField[ id ];
            sliceData[ x + nx * y ] = value;
        }
    }
    return sliceData;
}

std::vector<float> DatasetIsoline::extractAnatomySagittal( float x )
{
    std::vector<float>sliceData;
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    sliceData.resize( ny * nz, 0 );

    for ( int z = 0; z < nz; ++z )
    {
        for ( int y = 0; y < ny; ++y )
        {
            int id = getId( x, y, z );
            float value = m_scalarField[ id ];
            sliceData[ y + ny * z ] = value;
        }
    }
    return sliceData;
}

std::vector<float> DatasetIsoline::extractAnatomyCoronal( float y )
{
    std::vector<float>sliceData;
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    sliceData.resize( nx * nz, 0 );

    for ( int z = 0; z < nz; ++z )
    {
        for ( int x = 0; x < nx; ++x )
        {
            int id = getId( x, y, z );
            float value = m_scalarField[ id ];
            sliceData[ x + nx * z ] = value;
        }
    }
    return sliceData;
}

int DatasetIsoline::getId( int x, int y, int z )
{
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    int px = qMax( 0, qMin( x, nx - 1) );
    int py = qMax( 0, qMin( y, ny - 1) );
    int pz = qMax( 0, qMin( z, nz - 1) );

    return px + py * nx + pz * nx * ny;
}

void DatasetIsoline::isoValueChanged()
{
    m_dirty = true;
    Models::d()->submit();
}

void DatasetIsoline::globalChanged()
{
    if( m_x != Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat() ||
        m_y != Models::getGlobal( Fn::Property::G_CORONAL ).toFloat() ||
        m_z != Models::getGlobal( Fn::Property::G_AXIAL ).toFloat() )
    {
        m_dirty = true;
    }
    Models::d()->submit();
}
