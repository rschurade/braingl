/*
 * xmlreader.cpp
 *
 *  Created on: 05.08.2014
 *      Author: Ralph
 */

#include "xmlreader.h"

#include "../data/models.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>

XmlReader::XmlReader()
{
}

XmlReader::~XmlReader()
{
}

bool XmlReader::read( QIODevice* device )
{
    qDebug() << "xml reader read";
    m_xml.setDevice( device );

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();

        if ( isStart( "header" ) )
        {
            readHeader();
        }
        else if ( isStart( "scene" ) )
        {
            readScene();
        }
    }

    return !m_xml.error();
}

QVariant XmlReader::string2Variant( QString type, QString value )
{
    QStringList l = value.split( ' ' );
    bool ok = true;
    if ( type == "QMatrix4x4" )
    {
        QMatrix4x4 m;
        for ( int i = 0; i < 4; ++i )
        {
            for ( int j = 0; j < 4; ++j )
            {
                m( i, j ) = l[ i * 4 + j ].toFloat( &ok);
                if ( !ok )
                {
                    qCritical() << "XmlReader: Error parsing matrix " << value;
                }
            }
        }
        return m;
    }
    else if ( type == "QVector3D" )
    {
        QVector3D v;
        v.setX( l[0].toFloat( &ok ) );
        if ( !ok )
        {
            qCritical() << "XmlReader: Error parsing vector " << value;
        }
        v.setY( l[1].toFloat( &ok ) );
        if ( !ok )
        {
            qCritical() << "XmlReader: Error parsing vector " << value;
        }
        v.setZ( l[2].toFloat( &ok ) );
        if ( !ok )
        {
            qCritical() << "XmlReader: Error parsing vector " << value;
        }
        return v;
    }
    else if ( type == "QQuaternion" )
    {
        QQuaternion v;
        v.setX( l[0].toFloat( &ok ) );
        if ( !ok )
        {
            qCritical() << "XmlReader: Error parsing quaternion " << value;
        }
        v.setY( l[1].toFloat( &ok ) );
        if ( !ok )
        {
            qCritical() << "XmlReader: Error parsing quaternion " << value;
        }
        v.setZ( l[2].toFloat( &ok ) );
        if ( !ok )
        {
            qCritical() << "XmlReader: Error parsing quaternion " << value;
        }
        v.setScalar( l[3].toFloat( &ok ) );
        if ( !ok )
        {
            qCritical() << "XmlReader: Error parsing quaternion " << value;
        }
    }
    return value;
}

bool XmlReader::isStart( QString name )
{
    return ( m_xml.name() == name ) && m_xml.isStartElement();
}

bool XmlReader::isEnd( QString name )
{
    return ( m_xml.name() == name ) && m_xml.isEndElement();
}

bool XmlReader::readHeader()
{
    m_header.clear();
    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();
        if ( m_xml.isStartElement() )
        {
            QXmlStreamAttributes attribs = m_xml.attributes();
            if( attribs.hasAttribute( "name" ) )
            {
                QString name = attribs.value( "name" ).toString();
                m_header[name] = m_xml.readElementText();
                if( name == "content" )
                {
                    qDebug() << "reading braingGL " << m_header["content"].toString() << " file";
                }
            }

        }
        if ( isEnd( "header" ) )
        {
            break;
        }
    }

    return true;
}

bool XmlReader::readScene()
{
    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();
        if ( isStart( "globals" ) )
        {
            readGlobals();
        }
        else if ( isStart( "cameras" ) )
        {
            readCameras();
        }
        if ( isStart( "datasets" ) )
        {
            readDatasets();
        }
        else if ( isStart( "rois" ) )
        {
            readRois();
        }
    }

    return true;
}

bool XmlReader::readGlobals()
{
    m_globals.clear();

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();
        if ( isStart( "property" ) )
        {
            QXmlStreamAttributes attribs = m_xml.attributes();
            if( attribs.hasAttribute( "name" ) )
            {
                m_globals.push_back( (int)Models::s2p( attribs.value( "name" ).toString() ) );
                m_globals.push_back( string2Variant( attribs.value( "type" ).toString(), m_xml.readElementText() ) );
            }
        }
        if ( isEnd( "globals" ) )
        {
            break;
        }
    }


    return true;
}

bool XmlReader::readCameras()
{
    m_cameras.clear();
    QString name;
    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();
        if ( isStart( "camera" ) )
        {
            QXmlStreamAttributes attribs = m_xml.attributes();
            if( attribs.hasAttribute( "name" ) )
            {
                name = attribs.value( "name" ).toString();
                m_cameras[name] = readCamera();
            }
        }
        if ( isEnd( "cameras" ) )
        {
            break;
        }
    }


    return true;
}

QMap< QString, QVariant > XmlReader::readCamera()
{
    QMap< QString, QVariant >camera;
    QString name;
    QString value;
    QString type;

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();

        if ( isStart( "variant" ) )
        {
            QXmlStreamAttributes attribs = m_xml.attributes();
            if( attribs.hasAttribute( "name" ) )
            {
                name = attribs.value( "name" ).toString();
                type = attribs.value( "type" ).toString();
                value = m_xml.readElementText();
                camera[name] = string2Variant( type, value );
            }
        }
        if ( isEnd( "camera" ) )
        {
            break;
        }
    }

    return camera;
}

bool XmlReader::readDatasets()
{
    m_datasets.clear();

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();
        if ( isStart( "dataset" ) )
        {
            m_datasets.push_back( readDataset() );
        }
        if ( isEnd( "datasets" ) )
        {
            break;
        }
    }

    return true;
}

QList<QVariant> XmlReader::readDataset()
{
    QString name;
    QString value;
    QString type;
    QList<QVariant>dataset;

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();

        if ( isStart( "property" ) )
        {
            QXmlStreamAttributes attribs = m_xml.attributes();
            if( attribs.hasAttribute( "name" ) )
            {
                name = attribs.value( "name" ).toString();
                type = attribs.value( "type" ).toString();
                value = m_xml.readElementText();
                dataset.push_back( (int)Models::s2p( name ) );
                dataset.push_back( string2Variant( type, value ) );
            }
        }
        if ( isEnd( "dataset" ) )
        {
            break;
        }
    }
    return dataset;
}

bool XmlReader::readRois()
{
    m_rois.clear();

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();
        if ( isStart( "branch" ) )
        {
            m_rois.push_back( readBranch() );
        }
        if ( isEnd( "rois" ) )
        {
            break;
        }
    }


    return true;
}

QList< QList<QVariant> >XmlReader::readBranch()
{
    QList< QList<QVariant> >branch;

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();
        if ( isStart( "roi" ) )
        {
            branch.push_back( readRoi() );
        }
        if ( isEnd( "branch" ) )
        {
            break;
        }
    }
    return branch;
}

QList<QVariant> XmlReader::readRoi()
{
    QList<QVariant>roi;

    QString name;
    QString value;
    QString type;

    while ( !m_xml.atEnd() )
    {
        m_xml.readNext();

        if ( isStart( "property" ) )
        {
            QXmlStreamAttributes attribs = m_xml.attributes();
            if( attribs.hasAttribute( "name" ) )
            {
                name = attribs.value( "name" ).toString();
                type = attribs.value( "type" ).toString();
                value = m_xml.readElementText();
                roi.push_back( (int)Models::s2p( name ) );
                roi.push_back( string2Variant( type, value ) );
            }
        }
        if ( isEnd( "roi" ) )
        {
            break;
        }
    }
    return roi;
}

