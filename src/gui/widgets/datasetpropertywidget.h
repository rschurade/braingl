/*
 * datasetpropertywidget.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETPROPERTYWIDGET_H_
#define DATASETPROPERTYWIDGET_H_

#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

class DatasetPropertyView;
class QAbstractItemModel;
class QItemSelectionModel;
class QVBoxLayout;

class DatasetPropertyWidget : public QTabWidget
{
    Q_OBJECT

public:
    DatasetPropertyWidget( QWidget* parent = 0 );
    virtual ~DatasetPropertyWidget();

    void setModel( QAbstractItemModel* model );
    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    DatasetPropertyView* m_propertyView;

    QVBoxLayout* m_layout1;
    QVBoxLayout* m_layout2;
    QList<QWidget*>m_visibleWidgets1;
    QList<QWidget*>m_visibleWidgets2;

    QWidget* m_propWidget;
    QWidget* m_colormapWidget;

public slots:
    void updateWidgetVisibility();
    void colormapSelectionChanged( int id );
    void update();
};

#endif /* DATASETPROPERTYWIDGET_H_ */
