/*
 * xmlwriter.cpp
 *
 *  Created on: 04.08.2014
 *      Author: Ralph
 */

#include "xmlwriter.h"

#include <QIODevice>
#include <QMatrix4x4>
#include <QVector3D>

XmlWriter::XmlWriter()
{
}

XmlWriter::~XmlWriter()
{
}

void XmlWriter::beginWriteFile( QIODevice *device )
{
    m_xml.setDevice( device );
    m_xml.setAutoFormatting( true );

    m_xml.writeStartDocument();
    m_xml.writeDTD( "<!DOCTYPE xml>" );
    m_xml.writeStartElement( "xml" );
    m_xml.writeAttribute( "version", "1.0" );
}

void XmlWriter::endWriteFile()
{
    m_xml.writeEndDocument();
}

void XmlWriter::beginElement( QString name )
{
    m_xml.writeStartElement( name );
}

void XmlWriter::endElement()
{
    m_xml.writeEndElement();
}

void XmlWriter::writeAttribute( QString name, QString value )
{
    m_xml.writeAttribute( name, value );
}

void XmlWriter::writeQVariant( QString name, QVariant value )
{
    beginElement( "variant" );
    m_xml.writeAttribute( "name", name );
    m_xml.writeAttribute( "type", value.typeName() );

    m_xml.writeCharacters( variant2String( value ) );
    endElement();
}

void XmlWriter::writeProperty( Fn::Property name, QVariant value )
{
    beginElement( "property" );
    m_xml.writeAttribute( "name", Fn::Prop2String::e( name ) );
    m_xml.writeAttribute( "type", value.typeName() );

    m_xml.writeCharacters( variant2String( value ) );
    endElement();
}

void XmlWriter::writePropertyList( QString name, QList<QVariant> list )
{
    beginElement( name );
    m_xml.writeAttribute( "type", "list" );

    for( int i = 0; i < list.size(); ++i )
    {
        int n = list[i++].toInt();
        writeProperty(  (Fn::Property)n, list[i] );
    }

    endElement();
}

void XmlWriter::writeList( QString name, QList<QVariant> list )
{
    beginElement( name );
    m_xml.writeAttribute( "type", "list" );

    for( int i = 0; i < list.size(); ++i )
    {
        QString n = list[i++].toString();
        writeQVariant( n, list[i] );
    }

    endElement();
}

QString XmlWriter::variant2String( QVariant value )
{
    QString s;
    if ( value.type() == QVariant::Matrix4x4 )
    {
        QMatrix4x4 m = value.value<QMatrix4x4>();
        for ( int i = 0; i < 4; ++i )
        {
            for ( int j = 0; j < 4; ++j )
            {
                s += QString::number( m( i, j ) );
                s += " ";
            }
        }
    }
    else if ( value.type() == QVariant::Vector3D )
    {
        QVector3D v = value.value<QVector3D>();
        s += QString::number( v.x() );
        s += " ";
        s += QString::number( v.y() );
        s += " ";
        s += QString::number( v.z() );
    }
    else if ( value.type() == QVariant::Quaternion )
    {
        QQuaternion v = value.value<QQuaternion>();
        s += QString::number( v.x() );
        s += " ";
        s += QString::number( v.y() );
        s += " ";
        s += QString::number( v.z() );
        s += " ";
        s += QString::number( v.scalar() );
    }
    else
    {
        s = value.toString();
    }
    return s;
}
