/*
 * xmlreader.h
 *
 *  Created on: 05.08.2014
 *      Author: Ralph
 */

#ifndef XMLREADER_H_
#define XMLREADER_H_


#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QXmlStreamReader>

class XmlReader
{
public:
    XmlReader();
    virtual ~XmlReader();

    bool read( QIODevice* device );

    QVariant getHeaderValue( QString key ) { return m_header[key]; };
    QList<QVariant>getGlobals() { return m_globals; };
    QMap< QString, QMap< QString, QVariant > > getCameras() { return m_cameras; };
    QList< QList<QVariant> >getDatasets() { return m_datasets; };
    QList< QList< QList<QVariant> > >getRois() { return m_rois; };

    QList< QList<QVariant> >getColormaps() { return m_colormaps; };

private:
    QXmlStreamReader m_xml;
    QMap< QString, QVariant >m_header;
    QList<QVariant>m_globals;
    QMap< QString, QMap< QString, QVariant > >m_cameras;
    QList< QList<QVariant> >m_datasets;
    QList< QList< QList<QVariant> > >m_rois;

    QList< QList<QVariant> >m_colormaps;

    bool readHeader();
    bool readScene();

    bool readGlobals();

    bool readCameras();
    QMap< QString, QVariant > readCamera();

    bool readDatasets();
    QList<QVariant> readDataset();

    bool readRois();
    QList< QList<QVariant> >readBranch();
    QList<QVariant> readRoi();

    bool readColormaps();
    QList<QVariant> readColormap();


    bool isStart( QString name );
    bool isEnd( QString name );

    QVariant string2Variant( QString type, QString value );
};

#endif /* XMLREADER_H_ */
