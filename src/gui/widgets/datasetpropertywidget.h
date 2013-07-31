/*
 * datasetpropertywidget.h
 *
 * Created on: 13.05.2012
 * @author Ralph Schurade
 */

#ifndef DATASETPROPERTYWIDGET_H_
#define DATASETPROPERTYWIDGET_H_

#include <QString>
#include <QTabWidget>
#include <QWidget>

class DatasetPropertyView;
class QItemSelectionModel;
class QVBoxLayout;

class DatasetPropertyWidget : public QTabWidget
{
    Q_OBJECT

public:
    DatasetPropertyWidget( QString target, QWidget* parent = 0 );
    virtual ~DatasetPropertyWidget();

    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    QString m_target;

    DatasetPropertyView* m_propertyView;

public slots:
    void updateWidgetVisibility();
    void colormapSelectionChanged( int id );
    void update();
};

#endif /* DATASETPROPERTYWIDGET_H_ */
