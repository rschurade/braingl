/*
 * datasetinfowidget.h
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */

#ifndef DATASETINFOWIDGET_H_
#define DATASETINFOWIDGET_H_

#include "fndockwidget.h"

class QAbstractItemModel;
class QItemSelectionModel;
class DatasetInfoView;


class DatasetInfoWidget : public FNDockWidget
{
    Q_OBJECT

public:
    DatasetInfoWidget( QString name, QWidget* parent = 0 );
    virtual ~DatasetInfoWidget();

    void setModel( QAbstractItemModel* model );
    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    DatasetInfoView* m_infoView;
};

#endif /* DATASETINFOWIDGET_H_ */
