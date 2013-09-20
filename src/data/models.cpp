/*
 * models.cpp
 *
 * Created on: Mar 18, 2013
 * @author schurade
 */

#include "models.h"

#include "datastore.h"
#include "globalpropertymodel.h"
#include "roimodel.h"
#include "roi.h"
#include "vptr.h"

#include "datasets/dataset.h"

QAbstractItemModel* Models::m_dataModel = 0;
QAbstractItemModel* Models::m_globalModel = 0;
QAbstractItemModel* Models::m_roiModel = 0;

void Models::init()
{
    m_globalModel = new GlobalPropertyModel();
    m_dataModel = new DataStore();
    m_roiModel = new ROIModel();
}

QAbstractItemModel* Models::globalModel()
{
    return m_globalModel;
}

QAbstractItemModel* Models::dataModel()
{
    return m_dataModel;
}

QAbstractItemModel* Models::RoiModel()
{
    return m_roiModel;
}

QAbstractItemModel* Models::g()
{
    return m_globalModel;
}

QAbstractItemModel* Models::d()
{
    return m_dataModel;
}

QAbstractItemModel* Models::r()
{
    return m_roiModel;
}

void Models::setGlobal( int var, QVariant value )
{
    m_globalModel->setData( m_globalModel->index( var, 0 ), value );
}


void Models::setGlobal( Fn::Property var, QVariant value )
{
    m_globalModel->setData( m_globalModel->index( (int)var, 0 ), value );
}

QVariant Models::getGlobal( Fn::Property var )
{
    return m_globalModel->data( m_globalModel->index( (int)var, 0 ) );
}

QVariant Models::getGlobal( int var )
{
    return m_globalModel->data( m_globalModel->index( var, 0 ) );
}

ROI* Models::getRoi( int branch, int pos )
{
    ROI* roi = VPtr<ROI>::asPtr( m_roiModel->data( createRoiIndex( branch, pos, (int)Fn::Property::R_POINTER ), Qt::DisplayRole ) );
    return roi;
}

void Models::setROIProp( int branch, int pos, int prop, QVariant value )
{
    m_roiModel->setData( createRoiIndex( branch, pos, prop ), value, Qt::DisplayRole );
}

void Models::setROIProp( int branch, int pos, Fn::Property prop, QVariant value )
{
    m_roiModel->setData( createRoiIndex( branch, pos, (int)prop ), value, Qt::DisplayRole );
}


QModelIndex Models::createRoiIndex( int branch, int pos, int column )
{
    int row;
    QModelIndex parent;
    if ( pos == 0 )
    {
        row = branch;
    }
    else
    {
        row = pos - 1;
        parent = m_roiModel->index( branch, 0 );
    }
    return m_roiModel->index( row, column, parent );
}

QList<Dataset*> Models::getDatasets( Fn::DatasetType filter )
{
    QList<Dataset*>rl;
    int countDatasets = m_dataModel->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = m_dataModel->index( i, (int)Fn::Property::D_ACTIVE );
        if ( m_dataModel->data( index, Qt::DisplayRole ).toBool() )
        {
            index = m_dataModel->index( i, (int)Fn::Property::D_TYPE );
            if ( m_dataModel->data( index, Qt::DisplayRole ).toInt() & (int)filter )
            {
                Dataset* ds = VPtr<Dataset>::asPtr( m_dataModel->data( m_dataModel->index( i, (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
                rl.push_back( ds );
            }
        }
    }
    return rl;
}
