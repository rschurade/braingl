/*
 * statereader.h
 *
 *  Created on: 05.08.2014
 *      Author: Ralph
 */

#ifndef STATEREADER_H_
#define STATEREADER_H_

#include "xmlreader.h"

#include "../data/enums.h"

#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QVariant>

class ROIArea;

class StateReader
{
public:
    StateReader();
    virtual ~StateReader();

    void load( QString fileName );
    QMap< QString, QMap< QString, QVariant > > getCameras() { return m_cameras; };

private:
    XmlReader xml;
    QMap< QString, QMap< QString, QVariant > >m_cameras;
    QFileInfo m_fileInfo;

    void loadScene();
    void loadColormaps();

    QVariant getFromStateList( Fn::Property prop, QList<QVariant>& state );
    void loadDataset( QString fileName, QList<QVariant> state );
    ROIArea* loadRoi( QString fileName );
};

#endif /* STATEREADER_H_ */
