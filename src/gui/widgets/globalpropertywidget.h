/*
 * globalpropertywidget.h
 *
 * Created on: 13.05.2012
 * @author Ralph Schurade
 */

#ifndef GLOBALPROPERTYWIDGET_H_
#define GLOBALPROPERTYWIDGET_H_

#include <QWidget>
#include <QTabWidget>

class GlobalPropertyView;
class QAbstractItemModel;
class QItemSelectionModel;
class QVBoxLayout;
class SliderWithEditInt;

class GlobalPropertyWidget : public QTabWidget
{
    Q_OBJECT

public:
    GlobalPropertyWidget( QWidget* parent = 0 );
    virtual ~GlobalPropertyWidget();

    void setModel( QAbstractItemModel* model );

private:
    GlobalPropertyView* m_propertyView;

    QVBoxLayout* m_layout;


public slots:
    void dataChanged();

private slots:
    void sliderChanged( int value, int id );

};

#endif /* DATASETPROPERTYWIDGET_H_ */
