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

#include <QGLShaderProgram>

DatasetIsoline::DatasetIsoline( DatasetScalar* ds )  :
    Dataset( QDir( "new iso line" ), Fn::DatasetType::ISO_LINE ),
    m_dirty( true ),
    vbo0( 0 ),
    vbo1( 0 ),
    vbo2( 0 ),
    vbo3( 0 ),
    m_vertCountAxial( 0 ),
    m_vertCountCoronal( 0 ),
    m_vertCountSagittal( 0 ),
    m_stripeVertCountAxial( 0 ),
    m_stripeVertCountCoronal( 0 ),
    m_stripeVertCountSagittal( 0 ),
    m_x( 0 ),
    m_y( 0 ),
    m_z( 0 )
{
    m_scalarField = *(ds->getData() );

    m_properties["maingl"].createInt( Fn::Property::D_NX, ds->properties( "maingl" ).get( Fn::Property::D_NX ).toInt() );
    m_properties["maingl"].createInt( Fn::Property::D_NY, ds->properties( "maingl" ).get( Fn::Property::D_NY ).toInt() );
    m_properties["maingl"].createInt( Fn::Property::D_NZ, ds->properties( "maingl" ).get( Fn::Property::D_NZ ).toInt() );
    m_properties["maingl"].createFloat( Fn::Property::D_DX, ds->properties( "maingl" ).get( Fn::Property::D_DX ).toFloat(), "transform" );
    m_properties["maingl"].createFloat( Fn::Property::D_DY, ds->properties( "maingl" ).get( Fn::Property::D_DY ).toFloat(), "transform" );
    m_properties["maingl"].createFloat( Fn::Property::D_DZ, ds->properties( "maingl" ).get( Fn::Property::D_DZ ).toFloat(), "transform" );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_X, ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_X ).toFloat(), "transform" );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_Y, ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_Y ).toFloat(), "transform" );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_Z, ds->properties( "maingl" ).get( Fn::Property::D_ADJUST_Z ).toFloat(), "transform" );

    m_properties["maingl"].createInt( Fn::Property::D_DIM, 1 );
    m_properties["maingl"].createInt( Fn::Property::D_CREATED_BY, (int)Fn::Algo::ISOSURFACE );
    m_properties["maingl"].createInt( Fn::Property::D_TYPE, (int)Fn::DatasetType::ISO_LINE );

    QString name = ds->properties( "maingl" ).get( Fn::Property::D_NAME ).toString();
    if( !name.contains( "isoline" ) )
    {
        name += " (isoline)";
    }
    m_properties["maingl"].createString( Fn::Property::D_NAME, name );

    m_properties["maingl"].createBool( Fn::Property::D_RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_CORONAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_AXIAL, true, "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_SAGITTAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_CORONAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_AXIAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    m_properties["maingl"].createFloat( Fn::Property::D_LINE_WIDTH, 1, 1, 10, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_STIPPLE_GLYPH_SIZE, 1.2f, 1, 10, "general" );

    m_properties["maingl"].createColor( Fn::Property::D_COLOR, Models::getGlobal( Fn::Property::G_ISOLINE_STANDARD_COLOR ).value<QColor>(), "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    m_properties["maingl"].createBool( Fn::Property::D_INTERPOLATION, true, "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_INTERPOLATION ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    m_properties["maingl"].createFloat( Fn::Property::D_ISO_VALUE, 0.0f, ds->properties( "maingl" ).get( Fn::Property::D_MIN ).toFloat(), ds->properties( "maingl" ).get( Fn::Property::D_MAX ).toFloat(), "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_ISO_VALUE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    m_properties["maingl"].createList( Fn::Property::D_ISOLINE_STRIPES, { "none", "45 degrees right", "45 degrees left", "vertical", "horizontal", "dots" }, 0, "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_ISOLINE_STRIPES ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( isoValueChanged() ) );

    m_properties["maingl"].createFloat( Fn::Property::D_ISOLINE_STRIPES_WIDTH, 1, 1, 10, "general" );

    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );

    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( globalChanged() ) );
}

DatasetIsoline::~DatasetIsoline()
{
}

std::vector<float>* DatasetIsoline::getData()
{
    return &m_scalarField;
}

void DatasetIsoline::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    bool renderAnySlice =  m_properties["maingl"].get( Fn::Property::D_RENDER_AXIAL ).toBool() ||
                           m_properties["maingl"].get( Fn::Property::D_RENDER_CORONAL ).toBool() ||
                           m_properties["maingl"].get( Fn::Property::D_RENDER_SAGITTAL ).toBool();
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() || ! renderAnySlice )
    {
        return;
    }

    if ( m_dirty )
    {
        initGeometry();
    }

    float alpha = 1.0; //GLFunctions::sliceAlpha[target];

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

    QGLShaderProgram* program = GLFunctions::getShader( "stipple" );

    if ( ( m_vertCountAxial + m_vertCountCoronal + m_vertCountSagittal ) > 0 )
    {
        program->bind();

        intptr_t offset = 0;
        // Tell OpenGL programmable pipeline how to locate vertex position data
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo0 );

        int vertexLocation = program->attributeLocation( "a_position" );
        program->enableAttributeArray( vertexLocation );
        GLFunctions::f->glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );

        offset += sizeof(float) * 3;
        int offsetLocation = program->attributeLocation( "a_vec" );
        program->enableAttributeArray( offsetLocation );
        GLFunctions::f->glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );

        offset += sizeof(float) * 3;
        int dirLocation = program->attributeLocation( "a_dir2" );
        program->enableAttributeArray( dirLocation );
        GLFunctions::f->glVertexAttribPointer( dirLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
        int colorLocation = program->attributeLocation( "a_color" );
        program->enableAttributeArray( colorLocation );
        GLFunctions::f->glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0 );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        // Set modelview-projection matrix
        program->setUniformValue( "mvp_matrix", pMatrix * mvMatrix );
        program->setUniformValue( "u_scaling", 1.0f );

        program->setUniformValue( "u_alpha", 1.0f );
        program->setUniformValue( "u_renderMode", renderMode );
        program->setUniformValue( "u_canvasSize", width, height );
        program->setUniformValue( "D0", 9 );
        program->setUniformValue( "D1", 10 );
        program->setUniformValue( "D2", 11 );
        program->setUniformValue( "P0", 12 );

        program->setUniformValue( "u_aVec", 1., 0., 0. );
        program->setUniformValue( "u_bVec", 0., 1., 0. );
        program->setUniformValue( "u_orient", 0 );

        program->setUniformValue( "u_glyphThickness", m_properties["maingl"].get( Fn::Property::D_LINE_WIDTH ).toFloat() );
        program->setUniformValue( "u_glyphSize", m_properties["maingl"].get( Fn::Property::D_STIPPLE_GLYPH_SIZE ).toFloat() );
        program->setUniformValue( "u_constantThickness", true );

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_AXIAL ).toBool() && m_vertCountAxial > 0 )
        {
            glDrawArrays( GL_TRIANGLES, 0, m_vertCountAxial );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_CORONAL ).toBool() && m_vertCountCoronal > 0 )
        {
            program->setUniformValue( "u_aVec", 1., 0., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            program->setUniformValue( "u_orient", 1 );

            glDrawArrays( GL_TRIANGLES, m_vertCountAxial, m_vertCountCoronal );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_SAGITTAL ).toBool() && m_vertCountSagittal > 0 )
        {
            program->setUniformValue( "u_aVec", 0., 1., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            program->setUniformValue( "u_orient", 2 );

            glDrawArrays( GL_TRIANGLES, m_vertCountAxial + m_vertCountCoronal, m_vertCountSagittal );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }
    }

    if ( ( m_stripeVertCountAxial + m_stripeVertCountCoronal + m_stripeVertCountSagittal ) > 0 )
    {
        program->bind();

        intptr_t offset = 0;
        // Tell OpenGL programmable pipeline how to locate vertex position data
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo2 );

        int vertexLocation = program->attributeLocation( "a_position" );
        program->enableAttributeArray( vertexLocation );
        GLFunctions::f->glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );

        offset += sizeof(float) * 3;
        int offsetLocation = program->attributeLocation( "a_vec" );
        program->enableAttributeArray( offsetLocation );
        GLFunctions::f->glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );

        offset += sizeof(float) * 3;
        int dirLocation = program->attributeLocation( "a_dir2" );
        program->enableAttributeArray( dirLocation );
        GLFunctions::f->glVertexAttribPointer( dirLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *) offset );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo3 );
        int colorLocation = program->attributeLocation( "a_color" );
        program->enableAttributeArray( colorLocation );
        GLFunctions::f->glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0 );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        // Set modelview-projection matrix
        program->setUniformValue( "mvp_matrix", pMatrix * mvMatrix );
        program->setUniformValue( "u_scaling", 1.0f );

        program->setUniformValue( "u_alpha", 1.0f );
        program->setUniformValue( "u_renderMode", renderMode );
        program->setUniformValue( "u_canvasSize", width, height );
        program->setUniformValue( "D0", 9 );
        program->setUniformValue( "D1", 10 );
        program->setUniformValue( "D2", 11 );
        program->setUniformValue( "P0", 12 );

        program->setUniformValue( "u_aVec", 1., 0., 0. );
        program->setUniformValue( "u_bVec", 0., 1., 0. );
        program->setUniformValue( "u_orient", 0 );

        program->setUniformValue( "u_glyphThickness", m_properties["maingl"].get( Fn::Property::D_ISOLINE_STRIPES_WIDTH ).toFloat() );
        program->setUniformValue( "u_glyphSize", m_properties["maingl"].get( Fn::Property::D_STIPPLE_GLYPH_SIZE ).toFloat() );
        program->setUniformValue( "u_constantThickness", true );

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_AXIAL ).toBool() && m_stripeVertCountAxial > 0 )
        {
            glDrawArrays( GL_TRIANGLES, 0, m_stripeVertCountAxial );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_CORONAL ).toBool() && m_stripeVertCountCoronal > 0 )
        {
            program->setUniformValue( "u_aVec", 1., 0., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            program->setUniformValue( "u_orient", 1 );

            glDrawArrays( GL_TRIANGLES, m_stripeVertCountAxial, m_stripeVertCountCoronal );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_SAGITTAL ).toBool() && m_stripeVertCountSagittal > 0 )
        {
            program->setUniformValue( "u_aVec", 0., 1., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            program->setUniformValue( "u_orient", 2 );

            glDrawArrays( GL_TRIANGLES, m_stripeVertCountAxial + m_stripeVertCountCoronal, m_stripeVertCountSagittal );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }
    }
}

void DatasetIsoline::initGeometry()
{
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();
    float dx = m_properties["maingl"].get( Fn::Property::D_DX ).toFloat();
    float dy = m_properties["maingl"].get( Fn::Property::D_DY ).toFloat();
    float dz = m_properties["maingl"].get( Fn::Property::D_DZ ).toFloat();
    float ax = m_properties["maingl"].get( Fn::Property::D_ADJUST_X ).toFloat();
    float ay = m_properties["maingl"].get( Fn::Property::D_ADJUST_Y ).toFloat();
    float az = m_properties["maingl"].get( Fn::Property::D_ADJUST_Z ).toFloat();
    m_x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat();
    m_y = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat();
    m_z = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat();

    int xi = qMax( 0.0f, qMin( ( m_x - ax ) / dx, (float)nx - 1 ) );
    int yi = qMax( 0.0f, qMin( ( m_y - ay ) / dy, (float)ny - 1 ) );
    int zi = qMax( 0.0f, qMin( ( m_z - az ) / dz, (float)nz - 1 ) );

    float isoValue = m_properties["maingl"].get( Fn::Property::D_ISO_VALUE ).toFloat();
    bool interpolation = m_properties["maingl"].get( Fn::Property::D_INTERPOLATION ).toBool();
    m_color =  m_properties["maingl"].get( Fn::Property::D_COLOR ).value<QColor>();
    int stripeType = m_properties["maingl"].get( Fn::Property::D_ISOLINE_STRIPES ).toInt();

    std::vector<float>verts;
    std::vector<float>stripeVerts;
    std::vector<float>colors;
    std::vector<float>stripeColors;

    m_vertCountAxial = 0;
    m_vertCountCoronal = 0;
    m_vertCountSagittal = 0;

    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_AXIAL ).toBool() )
    {
        std::vector<float>sliceData;
        std::vector<float>tmpVerts;
        std::vector<float>tmpVerts2;

        sliceData = extractAnatomyAxial( zi );
        MarchingSquares ms1( &sliceData, isoValue, nx, ny, dx, dy, interpolation );
        tmpVerts = ms1.run();
        if ( stripeType > 0 )
        {
            tmpVerts2 = ms1.runStripes( stripeType, 2 );
        }

        if ( tmpVerts.size() > 0 )
        {
            for ( unsigned int i = 0; i < tmpVerts.size() / 4; ++i )
            {
                addGlyph( verts, colors, tmpVerts[4*i] + ax, tmpVerts[4*i+1] + ay, m_z, tmpVerts[4*i+2] + ax, tmpVerts[4*i+3] + ay, m_z );
            }
            if ( stripeType > 0 )
            {
                for ( unsigned int i = 0; i < tmpVerts2.size() / 4; ++i )
                {
                    addGlyph( stripeVerts, stripeColors, tmpVerts2[4*i] + ax, tmpVerts2[4*i+1] + ay, m_z, tmpVerts2[4*i+2] + ax, tmpVerts2[4*i+3] + ay, m_z );
                }
            }
            m_vertCountAxial = verts.size() / 8;
            m_stripeVertCountAxial = stripeVerts.size() / 8;
        }
    }

    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_CORONAL ).toBool() )
    {
        std::vector<float>sliceData;
        std::vector<float>tmpVerts;
        std::vector<float>tmpVerts2;

        sliceData = extractAnatomyCoronal( yi );
        MarchingSquares ms1( &sliceData, isoValue, nx, nz, dx, dz, interpolation );
        tmpVerts = ms1.run();
        if ( stripeType > 0 )
        {
            tmpVerts2 = ms1.runStripes( stripeType, 2 );
        }

        if ( tmpVerts.size() > 0 )
        {
            for ( unsigned int i = 0; i < tmpVerts.size() / 4; ++i )
            {
                addGlyph( verts, colors, tmpVerts[4*i] + ax, m_y, tmpVerts[4*i+1] + az, tmpVerts[4*i+2] + ax, m_y, tmpVerts[4*i+3] + az );
            }
            if ( stripeType > 0 )
            {
                for ( unsigned int i = 0; i < tmpVerts2.size() / 4; ++i )
                {
                    addGlyph( stripeVerts, stripeColors, tmpVerts2[4*i] + ax, m_y, tmpVerts2[4*i+1] + az, tmpVerts2[4*i+2] + ax, m_y, tmpVerts2[4*i+3] + az );
                }
            }
            m_vertCountCoronal = verts.size() / 8 - m_vertCountAxial;
            m_stripeVertCountCoronal = stripeVerts.size() / 8 - m_stripeVertCountAxial;
        }
    }

    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_SAGITTAL ).toBool() )
    {
        std::vector<float>sliceData;
        std::vector<float>tmpVerts;
        std::vector<float>tmpVerts2;

        sliceData = extractAnatomySagittal( xi );
        MarchingSquares ms1( &sliceData, isoValue, ny, nz, dy, dz, interpolation );
        tmpVerts = ms1.run();
        if ( stripeType > 0 )
        {
            tmpVerts2 = ms1.runStripes( stripeType, 2 );
        }

        if ( tmpVerts.size() > 0 )
        {
            for ( unsigned int i = 0; i < tmpVerts.size() / 4; ++i )
            {
                addGlyph( verts, colors, m_x, tmpVerts[4*i] + ay, tmpVerts[4*i+1] + az, m_x, tmpVerts[4*i+2] + ay, tmpVerts[4*i+3] + az );
            }
            if ( stripeType > 0 )
            {
                for ( unsigned int i = 0; i < tmpVerts2.size() / 4; ++i )
                {
                    addGlyph( stripeVerts, stripeColors, m_x, tmpVerts2[4*i] + ay, tmpVerts2[4*i+1] + az, m_x, tmpVerts2[4*i+2] + ay, tmpVerts2[4*i+3] + az );
                }
            }
            m_vertCountSagittal = verts.size() / 8 - ( m_vertCountCoronal + m_vertCountAxial );
            m_stripeVertCountSagittal = stripeVerts.size() / 8 - ( m_stripeVertCountCoronal + m_stripeVertCountAxial );
        }
    }

    if ( verts.size() > 0 )
    {
        if( vbo0 )
        {
            GLFunctions::f->glDeleteBuffers( 1, &vbo0 );
        }
        GLFunctions::f->glGenBuffers( 1, &vbo0 );
        if( vbo1 )
        {
            GLFunctions::f->glDeleteBuffers( 1, &vbo1 );
        }
        GLFunctions::f->glGenBuffers( 1, &vbo1 );
        if( vbo2 )
        {
            GLFunctions::f->glDeleteBuffers( 1, &vbo2 );
        }
        GLFunctions::f->glGenBuffers( 1, &vbo2 );
        if( vbo3 )
        {
            GLFunctions::f->glDeleteBuffers( 1, &vbo3 );
        }
        GLFunctions::f->glGenBuffers( 1, &vbo3 );


        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
        GLFunctions::f->glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof( float ), verts.data(), GL_DYNAMIC_DRAW );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
        GLFunctions::f->glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof( float ), colors.data(), GL_DYNAMIC_DRAW );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo2 );
        GLFunctions::f->glBufferData( GL_ARRAY_BUFFER, stripeVerts.size() * sizeof( float ), stripeVerts.data(), GL_DYNAMIC_DRAW );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo3 );
        GLFunctions::f->glBufferData( GL_ARRAY_BUFFER, stripeColors.size() * sizeof( float ), stripeColors.data(), GL_DYNAMIC_DRAW );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
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

void DatasetIsoline::addGlyph( std::vector<float> &verts, std::vector<float> &colors, float x1, float y1, float z1, float x2, float y2, float z2 )
{
    QVector3D s1( x1, y1, z1 );
    QVector3D s2( x2, y2, z2 );
    s1 = ( s1 + s2 ) / 2;
    s2 = s2 - s1;

    float xPos = s1.x();
    float yPos = s1.y();
    float zPos = s1.z();
    float v0 = s2.x();
    float v1 = s2.y();
    float v2 = s2.z();

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
    colors.push_back( 1.0f );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( 1.0f );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( 1.0f );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( 1.0f );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( 1.0f );

    colors.push_back( m_color.redF() );
    colors.push_back( m_color.greenF() );
    colors.push_back( m_color.blueF() );
    colors.push_back( 1.0f );
}

QString DatasetIsoline::getSaveFilter()
{
    return QString( "niftii (*.nii.gz);; all files (*.*)" );
}

QString DatasetIsoline::getDefaultSuffix()
{
    return QString( "nii.gz" );
}

