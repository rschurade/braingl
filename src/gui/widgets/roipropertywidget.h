/*
 * roipropertywidget.h
 *
 * Created on: 03.02.2013
 * @author Ralph Schurade
 */

#ifndef ROIPROPERTYWIDGET_H_
#define ROIPROPERTYWIDGET_H_

#include <QWidget>
#include <QTabWidget>

class ROIPropertyView;
class QAbstractItemModel;
class QItemSelectionModel;
class QVBoxLayout;

class ROIPropertyWidget : public QTabWidget
{
    Q_OBJECT

public:
    ROIPropertyWidget( QWidget* parent = 0 );
    virtual ~ROIPropertyWidget();

    void setModel( QAbstractItemModel* model );
    void setSelectionModel( QItemSelectionModel* selectionModel );

private:
    ROIPropertyView* m_propertyView;

    QVBoxLayout* m_layout;
    QList<QWidget*>m_visibleWidgets;

    bool m_buildingTabs;
    QString m_lastUsedTab;

public slots:
    void updateWidgetVisibility();
    void clearWidget();
    void slotTabChanged( int tab );
};

#endif /* ROIPROPERTYWIDGET_H_ */
