/*
 * datasetinfoview.cpp
 *
 * Created on: 12.05.2012
 * @author Ralph Schurade
 */
#include "datasetinfoview.h"

#include "delegates.h"

DatasetInfoView::DatasetInfoView(  QWidget * parent ) :
    QTableView( parent )
{
    setItemDelegateForColumn( 2, new NiftiFormatDelegate() );
    setItemDelegateForColumn( 3, new IntFormatDelegate() );
}

DatasetInfoView::~DatasetInfoView()
{
}

