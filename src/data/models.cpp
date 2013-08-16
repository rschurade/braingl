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

void Models::setGlobal( Fn::Property var, QVariant value )
{
    m_globalModel->setData( m_globalModel->index( (int)var, 0 ), value );
}

QVariant Models::getGlobal( Fn::Property var )
{
    return m_globalModel->data( m_globalModel->index( (int)var, 0 ) );
}

ROI* Models::getRoi( int branch, int pos )
{
    ROI* roi = VPtr<ROI>::asPtr( m_roiModel->data( createRoiIndex( branch, pos, (int)Fn::Property::R_POINTER ), Qt::DisplayRole ) );
    return roi;
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
