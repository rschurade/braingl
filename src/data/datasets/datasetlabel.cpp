/*
 * datasetlabel.cpp
 *
 *  Created on: 03.08.2014
 *      Author: Ralph Schurade
 */

#include "datasetlabel.h"

#include "../models.h"

#include "../../gui/gl/glfunctions.h"

#include <QtOpenGL/QGLShaderProgram>

DatasetLabel::DatasetLabel() :
    Dataset( QDir( "new label" ), Fn::DatasetType::LABEL ),
    m_handle0( GLFunctions::getPickIndex() )
{
    float x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat();
    float y = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat();
    float z = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat();

    QVector3D h0( x, y, z );

    m_properties["maingl"].createBool( Fn::Property::D_SHOW_PLANE_HANDLES, true, "general" );
    m_properties["maingl"].createColor( Fn::Property::D_HANDLE_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties["maingl"].createCharString( Fn::Property::D_COLORMAP_LABEL, "label", "general" );
    m_properties["maingl"].createInt( Fn::Property::D_COLORMAP_TEXT_SIZE, 50, 1, 200, "general" );

    m_properties["maingl"].createVector( Fn::Property::D_HANDLE_0, h0, "handles" );

    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"].createBool( Fn::Property::D_SHOW_PLANE_HANDLES, true, "general" );
    m_properties["maingl2"].createColor( Fn::Property::D_HANDLE_COLOR, QColor( 255, 0, 0 ), "general" );

}

DatasetLabel::~DatasetLabel()
{
}

void DatasetLabel::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    QVector3D h0 = m_properties["maingl"].get( Fn::Property::D_HANDLE_0 ).value<QVector3D>();
    QColor color = m_properties["maingl"].get( Fn::Property::D_HANDLE_COLOR ).value<QColor>();


    if ( m_properties["maingl"].get( Fn::Property::D_SHOW_PLANE_HANDLES ).toBool() )
    {
        color.setAlpha( 254 );
        GLFunctions::renderSphere( pMatrix, mvMatrix, h0.x(), h0.y(), h0.z(), 5, 5, 5,
                                  color, m_handle0, width, height, renderMode );
    }

    QString text =  m_properties["maingl"].get( Fn::Property::D_COLORMAP_LABEL ).toString();
    int size =  m_properties["maingl"].get( Fn::Property::D_COLORMAP_TEXT_SIZE ).toInt();
    GLFunctions::renderLabel( pMatrix, mvMatrix, text, size, h0.x(), h0.y(), h0.z(), color, 1.0, width, height, renderMode );
}

bool DatasetLabel::rightMouseDrag( int pickId, QVector3D dir, Qt::KeyboardModifiers modifiers, QString target )
{
    if ( pickId == m_handle0 )
    {
        QVector3D h0 = m_properties["maingl"].get( Fn::Property::D_HANDLE_0 ).value<QVector3D>();
        h0.setX( h0.x() + dir.x() );
        h0.setY( h0.y() + dir.y() );
        h0.setZ( h0.z() + dir.z() );

        m_properties["maingl"].set( Fn::Property::D_HANDLE_0, h0 );

        return true;
    }
    return false;
}
