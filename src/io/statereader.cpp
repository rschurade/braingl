/*
 * statereader.cpp
 *
 *  Created on: 05.08.2014
 *      Author: Ralph
 */

#include "statereader.h"

#include "loader.h"

#include "../data/globalpropertymodel.h"
#include "../data/models.h"
#include "../data/roiarea.h"
#include "../data/roibox.h"
#include "../data/vptr.h"

#include "../data/datasets/datasetguides.h"
#include "../data/datasets/datasetlabel.h"
#include "../data/datasets/datasetplane.h"
#include "../data/datasets/datasetscalar.h"

#include "../gui/gl/glfunctions.h"

#include <QDebug>
#include <QFile>

StateReader::StateReader()
{
}

StateReader::~StateReader()
{
}

void StateReader::load( QString fileName )
{
    m_fileInfo = QFileInfo( fileName );
    QFile file(fileName);
    if ( !file.open( QFile::ReadOnly | QFile::Text ) )
    {
        qCritical() << "StateReader: Cannot read file " << fileName << " : " << file.errorString();
        return;
    }
    xml.read( &file );

    if ( xml.getHeaderValue( "appName" ) != "brainGL" )
    {
        qCritical() << "Not a brainGL xml file";
        return;
    }

    if ( xml.getHeaderValue( "content" ).toString() == "scene" )
    {
        loadScene();
    }

    file.close();
}

void StateReader::loadScene()
{
    bool packAndGo = xml.getHeaderValue( "packAndGo" ).toBool();

    QList<QVariant>globals = xml.getGlobals();
    dynamic_cast<GlobalPropertyModel*>( Models::g() )->setState( globals );
    m_cameras = xml.getCameras();

    QList< QList<QVariant> >datasets = xml.getDatasets();
    for ( int i = 0; i < datasets.size(); ++i )
    {
        QList<QVariant> state = datasets[i];
        int type = getFromStateList( Fn::Property::D_TYPE, state ).toInt();
        if ( type == (int)Fn::DatasetType::GUIDE  )
        {
            DatasetGuides* g = new DatasetGuides();
            g->properties().setState( state );
            Models::addDataset( g );
        }
        else if ( type == (int)Fn::DatasetType::PLANE  )
        {
            DatasetPlane* plane = new DatasetPlane();
            plane->properties().setState( state );
            Models::addDataset( plane );
        }
        else if ( type == (int)Fn::DatasetType::LABEL  )
        {
            DatasetLabel* label = new DatasetLabel();
            label->properties().setState( state );
            Models::addDataset( label );
        }
        else
        {
            QString fn = getFromStateList( Fn::Property::D_FILENAME, state ).toString();
            if ( packAndGo )
            {
                QFileInfo fi( fn );
                QString fn = fi.fileName();
                loadDataset( m_fileInfo.path() + QDir::separator() + fn, state );
            }
            else
            {
                loadDataset( fn, state );
            }
        }
    }

    int numBranches = Models::r()->rowCount( QModelIndex() );
    QList< QList< QList<QVariant> > > rois = xml.getRois();
    for ( int i = 0; i < rois.size(); ++i )
    {
        QList< QList<QVariant> >branch = rois[i];

        QList<QVariant>roiState = branch[0];

        int shape = getFromStateList( Fn::Property::D_SHAPE, roiState ).toInt();

        if ( shape == 10 )
        {
            if ( packAndGo )
            {
                QFileInfo fi( getFromStateList( Fn::Property::D_FILENAME, roiState ).toString() );
                QString fn = fi.fileName();
                GLFunctions::roi = loadRoi( m_fileInfo.path() + QDir::separator() + fn );
            }
            else
            {
                QString fn = getFromStateList( Fn::Property::D_FILENAME, roiState ).toString();
                GLFunctions::roi = loadRoi( fn );
                GLFunctions::roi->properties()->setState( roiState );
            }
        }
        else
        {
            GLFunctions::roi = new ROIBox();
            GLFunctions::roi->properties()->setState( roiState );
        }

        for ( int l = 0; l < roiState.size() / 2; ++l )
        {
            if ( (Fn::Property)( roiState[l*2].toInt() ) == Fn::Property::D_COLOR )
            {
                GLFunctions::roi->properties()->set( (Fn::Property)( roiState[l*2].toInt() ), roiState[l*2+1] );
            }
        }

        Models::r()->insertRows( 0, 0, QModelIndex() );

        if ( branch.size() > 1 )
        {
            for ( int k = 1; k < branch.size(); ++k )
            {
                QList<QVariant>roiState = branch[k];

                int shape = getFromStateList( Fn::Property::D_SHAPE, roiState ).toInt();
                if ( shape == 10 )
                {
                    if ( packAndGo )
                    {
                        QFileInfo fi( getFromStateList( Fn::Property::D_FILENAME, roiState ).toString() );
                        QString fn = fi.fileName();
                        GLFunctions::roi = loadRoi( fi.path() + QDir::separator() + fn );
                    }
                    else
                    {
                        QString fn = getFromStateList( Fn::Property::D_FILENAME, roiState ).toString();
                        GLFunctions::roi = loadRoi( fn );
                        GLFunctions::roi->properties()->setState( roiState );
                    }
                    Models::r()->insertRows( 0, 0, Models::createRoiIndex( numBranches + i, 0, 0 ) );
                }
                else
                {
                    GLFunctions::roi = new ROIBox();
                    GLFunctions::roi->properties()->setState( roiState );
                    Models::r()->insertRows( 0, 0, Models::createRoiIndex( numBranches + i, 0, 0 ) );
                }
                for ( int l = 0; l < roiState.size() / 2; ++l )
                {
                    if ( (Fn::Property)( roiState[l*2].toInt() ) == Fn::Property::D_COLOR )
                    {
                        GLFunctions::roi->properties()->set( (Fn::Property)( roiState[l*2].toInt() ), roiState[l*2+1] );
                    }
                }
            }
        }
    }
}

QVariant StateReader::getFromStateList( Fn::Property prop, QList<QVariant>& state )
{
    for ( int i = 0; i < state.size() / 2; ++i )
    {
        if ( (Fn::Property)( state[i*2].toInt() ) == prop )
        {
            return state[i*2+1];
        }
    }
    return QVariant();
}

void StateReader::loadDataset( QString fileName, QList<QVariant> state )
{
    Loader loader;
    loader.setFilename( QDir( fileName ) );
    if ( loader.load() )
    {
        for ( int k = 0; k < loader.getNumDatasets(); ++k )
        {
            loader.getDataset( k )->properties().setState( state );
            Models::d()->setData( Models::d()->index( Models::d()->rowCount(), (int)Fn::Property::D_NEW_DATASET ), VPtr<Dataset>::asQVariant( loader.getDataset( k ) ), Qt::DisplayRole );
        }
        QFileInfo fi( fileName );
        QDir dir = fi.absoluteDir();
        QString lastPath = dir.absolutePath();
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ), lastPath );
    }
}

ROIArea* StateReader::loadRoi( QString fileName )
{
    Loader loader;
    loader.setFilename( QDir( fileName ) );
    if ( loader.load() )
    {
        DatasetScalar* dss = static_cast<DatasetScalar*>( loader.getDataset( 0 ) );
        std::vector<float>* data = dss->getData();
        std::vector<float> out( data->size(), 0.0 );

        for ( unsigned int i = 0; i < data->size(); ++i )
        {
            out[i] = data->at( i );
        }

        ROIArea* roiOut = new ROIArea( out, dss->properties() );

        QFileInfo fi( fileName );
        QDir dir = fi.absoluteDir();
        QString lastPath = dir.absolutePath();
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ), lastPath );

        return roiOut;
    }
    return NULL;
}
