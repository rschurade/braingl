/*
 * datasetpropertywidget.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef DATASETPROPERTYWIDGET_H_
#define DATASETPROPERTYWIDGET_H_

#include <QtGui/QWidget>

class DatasetPropertyView;
class QAbstractItemModel;
class QItemSelectionModel;
class QVBoxLayout;

class DatasetPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    DatasetPropertyWidget( QWidget* parent = 0 );
    virtual ~DatasetPropertyWidget();

    void setModel( QAbstractItemModel* model );
    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    DatasetPropertyView* m_propertyView;

    QVBoxLayout* m_layout;
    QList<QWidget*>m_visibleWidgets;

public slots:
    void updateWidgetVisibility();
};

#endif /* DATASETPROPERTYWIDGET_H_ */
