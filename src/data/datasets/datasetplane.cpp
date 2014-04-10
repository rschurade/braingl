/*
 * datasetplane.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: schurade
 */

#include "datasetplane.h"

#include "../models.h"

#include "../../gui/gl/glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>

DatasetPlane::DatasetPlane() :
    Dataset( QDir( "new plane" ), Fn::DatasetType::PLANE ),
    vbo0( 0 ),
    m_handle0( GLFunctions::getPickIndex() ),
    m_handle1( GLFunctions::getPickIndex() ),
    m_handle2( GLFunctions::getPickIndex() ),
    m_handle3( GLFunctions::getPickIndex() ),
    m_handle0Pos( 0, 0, 80 ),
    m_handle1Pos( 0, 200, 80 ),
    m_handle2Pos( 160, 200, 80 ),
    m_handle3Pos( 160, 0, 80 ),
    dirty( true )
{
}

DatasetPlane::~DatasetPlane()
{
}

void DatasetPlane::initProperties()
{
}

void DatasetPlane::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    GLFunctions::drawSphere( pMatrix, mvMatrix, m_handle0Pos.x(), m_handle0Pos.y(), m_handle0Pos.z(), 5, 5, 5,
                                   QColor( 255, 0, 0, 254 ), m_handle0, width, height, renderMode );
    GLFunctions::drawSphere( pMatrix, mvMatrix, m_handle1Pos.x(), m_handle1Pos.y(), m_handle1Pos.z(), 5, 5, 5,
                                       QColor( 255, 0, 0, 254 ), m_handle1, width, height, renderMode );
    GLFunctions::drawSphere( pMatrix, mvMatrix, m_handle2Pos.x(), m_handle2Pos.y(), m_handle2Pos.z(), 5, 5, 5,
                                       QColor( 255, 0, 0, 254 ), m_handle2, width, height, renderMode );
    GLFunctions::drawSphere( pMatrix, mvMatrix, m_handle3Pos.x(), m_handle3Pos.y(), m_handle3Pos.z(), 5, 5, 5,
                                       QColor( 255, 0, 0, 254 ), m_handle3, width, height, renderMode );
    if ( dirty )
    {
        initGeometry();
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

    if ( !GLFunctions::setupTextures() )
    {
        return;
    }
    //qDebug() << "main gl draw, renderMode:" << renderMode;
    glColor4f( 0.0, 0.0, 0.0, 1.0 );

    QGLShaderProgram* program = GLFunctions::getShader( "slice" );

    program->bind();
    glBindBuffer( GL_ARRAY_BUFFER, vbo0 );

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(float) * 3;
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation( "a_texcoord" );
    program->enableAttributeArray( texcoordLocation );
    glVertexAttribPointer( texcoordLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (const void *) offset );

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", pMatrix * mvMatrix );

    program->setUniformValue( "u_alpha", alpha );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );

    GLFunctions::setTextureUniforms( program, target );

    // Draw cube geometry using indices from VBO 0
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
void DatasetPlane::initGeometry()
{
    if( vbo0 )
    {
        glDeleteBuffers( 1, &vbo0 );
    }
    glGenBuffers( 1, &vbo0 );

    float x0 = m_handle0Pos.x();
    float y0 = m_handle0Pos.y();
    float z0 = m_handle0Pos.z();

    float x1 = m_handle1Pos.x();
    float y1 = m_handle1Pos.y();
    float z1 = m_handle1Pos.z();

    float x2 = m_handle2Pos.x();
    float y2 = m_handle2Pos.y();
    float z2 = m_handle2Pos.z();

    float x3 = m_handle3Pos.x();
    float y3 = m_handle3Pos.y();
    float z3 = m_handle3Pos.z();

    float nx = Models::getGlobal( Fn::Property::G_MAX_SAGITTAL ).toInt();
    float ny = Models::getGlobal( Fn::Property::G_MAX_CORONAL ).toInt();
    float nz = Models::getGlobal( Fn::Property::G_MAX_AXIAL ).toInt();

    float dx = Models::getGlobal( Fn::Property::G_SLICE_DX ).toFloat();
    float dy = Models::getGlobal( Fn::Property::G_SLICE_DY ).toFloat();
    float dz = Models::getGlobal( Fn::Property::G_SLICE_DZ ).toFloat();

    float vertices[] =
    {
        x0, y0, z0, x0 / ( nx * dx ), y0 / ( ny * dy ), z0 / ( nz * dz ),
        x1, y1, z1, x1 / ( nx * dx ), y1 / ( ny * dy ), z1 / ( nz * dz ),
        x3, y3, z3, x3 / ( nx * dx ), y3 / ( ny * dy ), z3 / ( nz * dz ),
        x2, y2, z2, x2 / ( nx * dx ), y2 / ( ny * dy ), z2 / ( nz * dz )
    };

    // Transfer vertex data to VBO 1
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo0 );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * 6 * sizeof(GLfloat), vertices, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    dirty = false;
}

bool DatasetPlane::rightMouseDrag( int pickId, QVector3D dir, Qt::KeyboardModifiers modifiers, QString target )
{
    if ( pickId == m_handle0 )
    {
        m_handle0Pos.setX( m_handle0Pos.x() + dir.x() );
        m_handle0Pos.setY( m_handle0Pos.y() + dir.y() );
        m_handle0Pos.setZ( m_handle0Pos.z() + dir.z() );
        dirty = true;
        return true;
    }
    else if ( pickId == m_handle1 )
    {
        m_handle1Pos.setX( m_handle1Pos.x() + dir.x() );
        m_handle1Pos.setY( m_handle1Pos.y() + dir.y() );
        m_handle1Pos.setZ( m_handle1Pos.z() + dir.z() );
        dirty = true;
        return true;
    }
    else if ( pickId == m_handle2 )
    {
        m_handle2Pos.setX( m_handle2Pos.x() + dir.x() );
        m_handle2Pos.setY( m_handle2Pos.y() + dir.y() );
        m_handle2Pos.setZ( m_handle2Pos.z() + dir.z() );
        dirty = true;
        return true;
    }

    else if ( pickId == m_handle3 )
    {
        m_handle3Pos.setX( m_handle3Pos.x() + dir.x() );
        m_handle3Pos.setY( m_handle3Pos.y() + dir.y() );
        m_handle3Pos.setZ( m_handle3Pos.z() + dir.z() );
        dirty = true;
        return true;
    }

    return false;
}
