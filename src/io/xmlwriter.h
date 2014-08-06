/*
 * xmlwriter.h
 *
 *  Created on: 04.08.2014
 *      Author: Ralph
 */

#ifndef XMLWRITER_H_
#define XMLWRITER_H_

#include "../data/enums.h"

#include <QList>
#include <QString>
#include <QVariant>
#include <QXmlStreamWriter>

class QIODevice;

class XmlWriter
{
public:
    XmlWriter();
    virtual ~XmlWriter();

    void beginWriteFile( QIODevice* device );
    void endWriteFile();

    void beginElement( QString name );
    void endElement();

    void writeAttribute( QString name, QString value );

    void writeString( QString name, QString value );
    void writeQVariant( QString name, QVariant value );

    void writeProperty( Fn::Property, QVariant value );
    void writePropertyList( QString name, QList<QVariant> list );
    void writeList( QString name, QList<QVariant> list );



private:
    QXmlStreamWriter m_xml;

    QString variant2String( QVariant value );

};

#endif /* XMLWRITER_H_ */
