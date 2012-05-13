/*
 * datasetpropertywidget.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef DATASETPROPERTYWIDGET_H_
#define DATASETPROPERTYWIDGET_H_

#include <QtGui/QDockWidget>

class DatasetPropertyView;

class DatasetPropertyWidget : public QDockWidget
{
    Q_OBJECT

public:
    DatasetPropertyWidget( QString name, QWidget* parent = 0 );
    virtual ~DatasetPropertyWidget();

    void setModel( QAbstractItemModel* model );
    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    DatasetPropertyView* m_propertyView;
};

#endif /* DATASETPROPERTYWIDGET_H_ */
