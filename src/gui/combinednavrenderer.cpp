/*
 * NavRenderer.cpp
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */
#include "combinednavrenderer.h"

#include <QtCore/QDebug>

#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../data/datastore.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

CombinedNavRenderer::CombinedNavRenderer( QString name ) :
    ObjectRenderer(),
    m_name( name ),
    m_ratio( 1.0 ),
    m_program( new QGLShaderProgram ),
    vboIds( new GLuint[ 2 ] ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_xb( 0 ),
    m_yb( 0 ),
    m_zb( 0 )
{
}

CombinedNavRenderer::~CombinedNavRenderer()
{
}

void CombinedNavRenderer::init()
{

}

void CombinedNavRenderer::initGL()
{
    glClearColor( 1.0, 0.0, 1.0, 0.0 );

    glEnable( GL_DEPTH_TEST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );
    static GLfloat lightPosition[ 4 ] = { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    initShader();
}

void CombinedNavRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;

    adjustRatios();
}

void CombinedNavRenderer::adjustRatios()
{
    m_ratio = static_cast< float >( m_width ) / static_cast< float >( m_height );
    glViewport( 0, 0, m_width, m_height );

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();

    if ( m_ratio > 1.5 )
    {
        float textureRatio =  ( m_xb + m_xb + m_yb ) / m_yb;
        float mult = textureRatio / m_ratio;
        if ( m_ratio > textureRatio )
        {
            pMatrix.ortho( 0, ( m_xb + m_xb + m_yb ) / mult, 0, m_yb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_xb + m_xb + m_yb, 0, m_yb * mult , -3000, 3000 );
        }
    }
    else if ( m_ratio < 0.66 )
    {
        float textureRatio = m_yb / ( m_yb + m_zb + m_zb );
        float mult = textureRatio / m_ratio;
        //qDebug() << "ratio: " << m_ratio << " trat: " << textureRatio << " mult: " << mult;
        if ( m_ratio > textureRatio )
        {
            pMatrix.ortho( 0, m_yb / mult, 0, ( m_yb + m_zb + m_zb ), -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_yb, 0, ( m_yb + m_zb + m_zb ) * mult, -3000, 3000 );
        }


    }
    else
    {
        float mult = 1.0 / m_ratio;
        //qDebug() << "ratio: " << m_ratio << " trat: " << textureRatio << " mult: " << mult;
        if ( m_ratio > 1.0 )
        {
            pMatrix.ortho( 0, ( m_xb + m_yb ) / mult, 0, m_xb + m_yb, -3000, 3000 );
        }
        else
        {
            pMatrix.ortho( 0, m_xb + m_yb, 0, ( m_xb + m_yb ) * mult, -3000, 3000 );
        }
    }

     m_mvpMatrix = pMatrix;
}

void CombinedNavRenderer::leftMouseDown( int x, int y )
{
}

void CombinedNavRenderer::leftMouseDrag( int x, int y )
{
}

void CombinedNavRenderer::initShader()
{
    // Overriding system locale until shaders are compiled
    setlocale( LC_NUMERIC, "C" );

    // Compiling vertex shader
    if ( !m_program->addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/vshader.glsl" ) )
    {
        exit( false );
    }

    // Compiling fragment shader
    if ( !m_program->addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/fshader.glsl" ) )
    {
        exit( false );
    }

    // Linking shader pipeline
    if ( !m_program->link() )
    {
        exit( false );
    }

    // Binding shader pipeline for use
    if ( !m_program->bind() )
    {
        exit( false );
    }

    // Restore system locale
    setlocale( LC_ALL, "" );
}

void CombinedNavRenderer::initGeometry()
{
    glGenBuffers( 2, vboIds );

    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();
    m_xb = model()->data( model()->index( 0, 103 ), Qt::UserRole ).toFloat();
    m_yb = model()->data( model()->index( 0, 104 ), Qt::UserRole ).toFloat();
    m_zb = model()->data( model()->index( 0, 105 ), Qt::UserRole ).toFloat();

    float dx = model()->data( model()->index( 0, 106 ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, 107 ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, 108 ), Qt::UserRole ).toFloat();

    m_x *= dx;
    m_y *= dy;
    m_z *= dz;
    m_xb *= dx;
    m_yb *= dy;
    m_zb *= dz;

    VertexData vertices[12];

    if ( m_ratio > 1.5 )
    {
        VertexData vd0 = { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) };

        VertexData vd1 =  { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) };
        VertexData vd2 =  { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) };
        VertexData vd3 =  { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) };

        VertexData vd4 =  { QVector3D( m_xb,  0.0,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) };
        VertexData vd5 =  { QVector3D( m_xb + m_xb, 0.0,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) };
        VertexData vd6 =  { QVector3D( m_xb + m_xb, m_zb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) };
        VertexData vd7 =  { QVector3D( m_xb,  m_zb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) };

        VertexData vd8 =  { QVector3D( m_xb + m_xb,  0.0,  m_x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) };
        VertexData vd9 =  { QVector3D( m_xb + m_xb + m_yb, 0.0,  m_x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) };
        VertexData vd10 = { QVector3D( m_xb + m_xb + m_yb, m_zb, m_x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) };
        VertexData vd11 = { QVector3D( m_xb + m_xb,  m_zb, m_x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) };
        vertices[ 0 ] = vd0;
        vertices[ 1 ] = vd1;
        vertices[ 2 ] = vd2;
        vertices[ 3 ] = vd3;
        vertices[ 4 ] = vd4;
        vertices[ 5 ] = vd5;
        vertices[ 6 ] = vd6;
        vertices[ 7 ] = vd7;
        vertices[ 8 ] = vd8;
        vertices[ 9 ] = vd9;
        vertices[ 10 ] = vd10;
        vertices[ 11 ] = vd11;
    }
    else if ( m_ratio < 0.66 )
    {
        VertexData vd0 =  { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) };
        VertexData vd1 =  { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) };
        VertexData vd2 =  { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) };
        VertexData vd3 =  { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) };

        VertexData vd4 =  { QVector3D( 0.0,  m_yb,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) };
        VertexData vd5 =  { QVector3D( m_xb, m_yb,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) };
        VertexData vd6 =  { QVector3D( m_xb, m_yb + m_zb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) };
        VertexData vd7 =  { QVector3D( 0.0,  m_yb + m_zb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) };

        VertexData vd8 =  { QVector3D( 0.0,  m_yb + m_zb,  m_x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) };
        VertexData vd9 =  { QVector3D( m_yb, m_yb + m_zb,  m_x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) };
        VertexData vd10 = { QVector3D( m_yb, m_yb + m_zb + m_zb, m_x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) };
        VertexData vd11 = { QVector3D( 0.0,  m_yb + m_zb + m_zb, m_x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) };
        vertices[ 0 ] = vd0;
        vertices[ 1 ] = vd1;
        vertices[ 2 ] = vd2;
        vertices[ 3 ] = vd3;
        vertices[ 4 ] = vd4;
        vertices[ 5 ] = vd5;
        vertices[ 6 ] = vd6;
        vertices[ 7 ] = vd7;
        vertices[ 8 ] = vd8;
        vertices[ 9 ] = vd9;
        vertices[ 10 ] = vd10;
        vertices[ 11 ] = vd11;
    }
    else
    {
        VertexData vd0 =  { QVector3D( 0.0,  0.0,  m_z ), QVector3D( 0.0, 0.0, m_z/m_zb ) };
        VertexData vd1 =  { QVector3D( m_xb, 0.0,  m_z ), QVector3D( 1.0, 0.0, m_z/m_zb ) };
        VertexData vd2 =  { QVector3D( m_xb, m_yb, m_z ), QVector3D( 1.0, 1.0, m_z/m_zb ) };
        VertexData vd3 =  { QVector3D( 0.0,  m_yb, m_z ), QVector3D( 0.0, 1.0, m_z/m_zb ) };

        VertexData vd4 =  { QVector3D( 0.0,  m_yb,  m_y ), QVector3D( 0.0, m_y/m_yb, 0.0 ) };
        VertexData vd5 =  { QVector3D( m_xb, m_yb,  m_y ), QVector3D( 1.0, m_y/m_yb, 0.0 ) };
        VertexData vd6 =  { QVector3D( m_xb, m_zb + m_yb, m_y ), QVector3D( 1.0, m_y/m_yb, 1.0 ) };
        VertexData vd7 =  { QVector3D( 0.0,  m_zb + m_yb, m_y ), QVector3D( 0.0, m_y/m_yb, 1.0 ) };

        VertexData vd8 =  { QVector3D( m_xb,  m_yb,  m_x ), QVector3D( m_x/m_xb, 0.0, 0.0 ) };
        VertexData vd9 =  { QVector3D( m_yb + m_xb, m_yb,  m_x ), QVector3D( m_x/m_xb, 1.0, 0.0 ) };
        VertexData vd10 = { QVector3D( m_yb + m_xb, m_zb + m_yb, m_x ), QVector3D( m_x/m_xb, 1.0, 1.0 ) };
        VertexData vd11 = { QVector3D( m_xb,  m_zb + m_yb, m_x ), QVector3D( m_x/m_xb, 0.0, 1.0 ) };
        vertices[ 0 ] = vd0;
        vertices[ 1 ] = vd1;
        vertices[ 2 ] = vd2;
        vertices[ 3 ] = vd3;
        vertices[ 4 ] = vd4;
        vertices[ 5 ] = vd5;
        vertices[ 6 ] = vd6;
        vertices[ 7 ] = vd7;
        vertices[ 8 ] = vd8;
        vertices[ 9 ] = vd9;
        vertices[ 10 ] = vd10;
        vertices[ 11 ] = vd11;
    }

    GLushort indices[] = { 0, 1, 2, 0, 2, 3,
                           4, 5, 6, 4, 6, 7,
                           8, 9, 10, 8, 10, 11};

    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 18 * sizeof(GLushort), indices, GL_STATIC_DRAW );

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(VertexData), vertices, GL_STATIC_DRAW );
}

void CombinedNavRenderer::setupTextures()
{
    int countDatasets = model()->rowCount();
    int allocatedTextureCount = 0;

    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model()->index( i, 1 );
        if ( model()->data( index, Qt::DisplayRole ).toInt() == 1 || model()->data( index, Qt::DisplayRole ).toInt() == 3 )
        {
            GLuint tex = static_cast<GLuint>( model()->data( index, Qt::UserRole ).toInt() );

            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

            glActiveTexture( GL_TEXTURE0 + allocatedTextureCount );
            glBindTexture( GL_TEXTURE_3D, tex );

            bool interpolation = false;
            if ( interpolation )
            {
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            }
            else
            {
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            }
            ++allocatedTextureCount;
        }
    }
}

void CombinedNavRenderer::setShaderVars()
{
    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation( "a_position" );
    m_program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = m_program->attributeLocation("a_texcoord");
    m_program->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
}

void CombinedNavRenderer::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setupTextures();

    adjustRatios();


    // Set modelview-projection matrix
    m_program->setUniformValue( "mvp_matrix", m_mvpMatrix );

    initGeometry();

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();

    // Draw cube geometry using indices from VBO 0
    glDrawElements( GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0 );



    glDeleteBuffers( 1, &vboIds[0] );
    glDeleteBuffers( 1, &vboIds[1] );
}
