/*
 * datasetpropertywidget.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETPROPERTYWIDGET_H_
#define DATASETPROPERTYWIDGET_H_

#include <QTabWidget>
#include <QWidget>

class DatasetPropertyView;
class QItemSelectionModel;
class QVBoxLayout;

class DatasetPropertyWidget : public QTabWidget
{
    Q_OBJECT

public:
    DatasetPropertyWidget( QWidget* parent = 0 );
    virtual ~DatasetPropertyWidget();

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
