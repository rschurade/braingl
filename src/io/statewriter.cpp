/*
 * statewriter.cpp
 *
 *  Created on: 05.08.2014
 *      Author: Ralph
 */

#include "statewriter.h"

#include "xmlwriter.h"

#include "../data/globalpropertymodel.h"
#include "../data/models.h"
#include "../data/roi.h"
#include "../data/vptr.h"

#include "../data/datasets/dataset.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

StateWriter::StateWriter()
{
}

StateWriter::~StateWriter()
{
}

void StateWriter::saveScene( QString fileName, bool packAndGo, QList<QVariant>camera1, QList<QVariant>arcball1, QList<QVariant>camera2, QList<QVariant>arcball2 )
{
    QFileInfo fi( fileName );
    QString newScenePath = fi.path();

    XmlWriter writer;
    QFile file( fileName );
    if ( !file.open( QFile::WriteOnly | QFile::Text ) )
    {

        qDebug() << "Cannot write file " + fileName + " : " + file.errorString();
        return;
    }

    writer.beginWriteFile( &file );

    writer.beginElement( "header" );
    writer.writeQVariant( "appName", "brainGL" );
    writer.writeQVariant( "url", "http://code.google.com/p/braingl/" );
    writer.writeQVariant( "version", "0.9.1" );
    writer.writeQVariant( "content", "scene" );
    writer.writeQVariant( "packAndGo", packAndGo );
    writer.writeQVariant( "date", QDateTime::currentDateTime().toString( "yyyy.MM.dd" ) );
    writer.writeQVariant( "time", QDateTime::currentDateTime().toString( "hh:mm:ss" ) );

    writer.endElement(); // header

    writer.beginElement( "scene" );

    writer.writePropertyList( "globals", dynamic_cast<GlobalPropertyModel*>( Models::g() )->getState() );

    writer.beginElement( "cameras" );

    writer.beginElement( "camera" );
    writer.writeAttribute( "name", "camera_maingl" );
    writer.writeList( "properties", camera1 );
    writer.endElement(); // camera

    writer.beginElement( "camera" );
    writer.writeAttribute( "name", "arcball_maingl" );
    writer.writeList( "properties", arcball1 );
    writer.endElement(); // camera

    writer.beginElement( "camera" );
    writer.writeAttribute( "name", "camera_maingl2" );
    writer.writeList( "properties", camera2 );
    writer.endElement(); // camera

    writer.beginElement( "camera" );
    writer.writeAttribute( "name", "arcball_maingl2" );
    writer.writeList( "properties", arcball2 );
    writer.endElement(); // camera

    writer.endElement(); // cameras


    writer.beginElement( "datasets" );
    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        writer.beginElement( "dataset" );
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int) Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
        QList<QVariant>state = ds->properties().getState();
        writer.writePropertyList( "properties", state );
        writer.endElement(); // dataset
    }
    writer.endElement(); // datasets

    writer.beginElement( "rois" );
    int numBranches = Models::r()->rowCount( QModelIndex() );
    QList<QVariant> roiStates;

    for ( int i = 0; i < numBranches; ++i )
    {
        writer.beginElement( "branch" );
        writer.writeAttribute( "position", QString::number( i ) );
        int leafCount = Models::getBranchSize( i );

        for ( int k = 0; k < leafCount + 1; ++k )
        {
            writer.beginElement( "roi" );
            writer.writeAttribute( "position", QString::number( k ) );
            ROI* roi = Models::getRoi( i, k );
            QList<QVariant> state = roi->properties()->getState();
            writer.writePropertyList( "properties", state );
            writer.endElement(); // branch
        }
        writer.endElement(); // branch
    }
    writer.endElement(); // rois

    writer.endElement(); // scene
    writer.endWriteFile();

    file.close();

}
