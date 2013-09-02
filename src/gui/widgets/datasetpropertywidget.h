/*
 * datasetpropertywidget.h
 *
 * Created on: 13.05.2012
 * @author Ralph Schurade
 */

#ifndef DATASETPROPERTYWIDGET_H_
#define DATASETPROPERTYWIDGET_H_

#include <QHash>
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

    QHash<QString, QVBoxLayout*>m_tabs;
    QString m_lastUsedTab;
    bool m_buildingTabs;

public slots:
    void updateWidgetVisibility();
    void colormapSelectionChanged( int id );
    void slotTabChanged( int tab );
    void update();
};

#endif /* DATASETPROPERTYWIDGET_H_ */
