/*
 * datasetguides.cpp
 *
 *  Created on: 08.07.2014
 *      Author: Ralph
 */

#include "datasetguides.h"

#include "datasetscalar.h"

#include "../enums.h"
#include "../models.h"

#include "../../algos/marchingsquares.h"

#include "../../gui/gl/glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>


DatasetGuides::DatasetGuides() :
    Dataset( QDir( "new guide" ), Fn::DatasetType::GUIDE ),
    m_dirty( true ),
    vbo0( 0 ),
    vbo1( 0 ),
    m_vertCountAxial( 0 ),
    m_vertCountCoronal( 0 ),
    m_vertCountSagittal( 0 ),
    m_x( 0 ),
    m_y( 0 ),
    m_z( 0 )
{
    m_properties["maingl"].createInt( Fn::Property::D_DIM, 0 );
    m_properties["maingl"].createInt( Fn::Property::D_CREATED_BY, (int)Fn::Algo::NONE );

    QString name = "new guide";
    m_properties["maingl"].createString( Fn::Property::D_NAME, name );


    m_properties["maingl"].createFloat( Fn::Property::D_LINE_WIDTH, 1, 1, 10, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_STIPPLE_GLYPH_SIZE, 1.2f, 1, 10 );

    m_properties["maingl"].createColor( Fn::Property::D_COLOR, QColor( 200, 200, 200 ), "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );

    m_properties["maingl"].createBool( Fn::Property::D_STICK_TO_CROSSHAIR, true, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_GUIDE_X, 0, -250, 250, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_GUIDE_Y, 0, -250, 250, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_GUIDE_Z, 0, -250, 250, "general" );
    m_properties["maingl"].set( Fn::Property::D_GUIDE_X, Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat() );
    m_properties["maingl"].set( Fn::Property::D_GUIDE_Y, Models::getGlobal( Fn::Property::G_CORONAL ).toFloat() );
    m_properties["maingl"].set( Fn::Property::D_GUIDE_Z, Models::getGlobal( Fn::Property::G_AXIAL ).toFloat() );

    connect( m_properties["maingl"].getProperty( Fn::Property::D_STICK_TO_CROSSHAIR ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_GUIDE_X ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_GUIDE_Y ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_GUIDE_Z ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );

    m_properties["maingl"].createBool( Fn::Property::D_RENDER_SAGITTAL, true, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_CORONAL, true, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_AXIAL, true, "general" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_SAGITTAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_CORONAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_AXIAL ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( globalChanged() ) );



    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );

    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( globalChanged() ) );
}

DatasetGuides::~DatasetGuides()
{
}

void DatasetGuides::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
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

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_AXIAL ).toBool() )
        {
            glDrawArrays( GL_TRIANGLES, 0, m_vertCountAxial );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_CORONAL ).toBool() )
        {
            program->setUniformValue( "u_aVec", 1., 0., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            program->setUniformValue( "u_orient", 1 );

            glDrawArrays( GL_TRIANGLES, m_vertCountAxial, m_vertCountCoronal );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }

        if ( m_properties["maingl"].get( Fn::Property::D_RENDER_SAGITTAL ).toBool() )
        {
            program->setUniformValue( "u_aVec", 0., 1., 0. );
            program->setUniformValue( "u_bVec", 0., 0., 1. );
            program->setUniformValue( "u_orient", 2 );

            glDrawArrays( GL_TRIANGLES, m_vertCountAxial + m_vertCountCoronal, m_vertCountSagittal );
            GLFunctions::getAndPrintGLError( "render stipples: opengl error" );
        }
    }
}

void DatasetGuides::initGeometry()
{
    if ( m_properties["maingl"].get( Fn::Property::D_STICK_TO_CROSSHAIR ).toBool() )
    {
        m_x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat();
        m_y = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat();
        m_z = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat();
    }
    else
    {
        m_x = m_properties["maingl"].get( Fn::Property::D_GUIDE_X ).toFloat();
        m_y = m_properties["maingl"].get( Fn::Property::D_GUIDE_Y ).toFloat();
        m_z = m_properties["maingl"].get( Fn::Property::D_GUIDE_Z ).toFloat();
    }

    m_color =  m_properties["maingl"].get( Fn::Property::D_COLOR ).value<QColor>();

    std::vector<float>verts;
    std::vector<float>colors;

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

    m_vertCountAxial = 0;
    m_vertCountCoronal = 0;
    m_vertCountSagittal = 0;

    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_AXIAL ).toBool() )
    {
        for ( int i = -250; i < 250; ++i )
        {
            addGlyph( verts, colors, i, m_y, m_z, i + 1, m_y, m_z );
        }

        for ( int i = -250; i < 250; ++i )
        {
            addGlyph( verts, colors, m_x, i, m_z, m_x, i + 1, m_z );
        }

        m_vertCountAxial = verts.size() / 8;
    }

    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_CORONAL ).toBool() )
    {
        for ( int i = -250; i < 250; ++i )
        {
            addGlyph( verts, colors, i, m_y, m_z, i + 1, m_y, m_z );
        }

        for ( int i = -250; i < 250; ++i )
        {
            addGlyph( verts, colors, m_x, m_y, i, m_x, m_y, i + 1 );
        }

        m_vertCountCoronal = verts.size() / 8 - m_vertCountAxial;
    }

    if ( m_properties["maingl"].get( Fn::Property::D_RENDER_SAGITTAL ).toBool() )
    {
        for ( int i = -250; i < 250; ++i )
        {
            addGlyph( verts, colors, m_x, i, m_z, m_x, i + 1, m_z );
        }

        for ( int i = -250; i < 250; ++i )
        {
            addGlyph( verts, colors, m_x, m_y, i, m_x, m_y, i + 1 );
        }

        m_vertCountSagittal = verts.size() / 8 - ( m_vertCountCoronal + m_vertCountAxial );
    }
    if ( verts.size() > 0 )
    {
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo0 );
        GLFunctions::f->glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof( float ), verts.data(), GL_DYNAMIC_DRAW );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, vbo1 );
        GLFunctions::f->glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof( float ), colors.data(), GL_DYNAMIC_DRAW );
        GLFunctions::f->glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    m_dirty = false;
}

void DatasetGuides::globalChanged()
{
    if ( m_properties["maingl"].get( Fn::Property::D_STICK_TO_CROSSHAIR ).toBool() )
    {
        m_properties["maingl"].set( Fn::Property::D_GUIDE_X, Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat() );
        m_properties["maingl"].set( Fn::Property::D_GUIDE_Y, Models::getGlobal( Fn::Property::G_CORONAL ).toFloat() );
        m_properties["maingl"].set( Fn::Property::D_GUIDE_Z, Models::getGlobal( Fn::Property::G_AXIAL ).toFloat() );
    }

    m_dirty = true;
    Models::d()->submit();
}

void DatasetGuides::addGlyph( std::vector<float> &verts, std::vector<float> &colors, float x1, float y1, float z1, float x2, float y2, float z2 )
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

