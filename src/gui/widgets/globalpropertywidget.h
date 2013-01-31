/*
 * globalpropertywidget.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef GLOBALPROPERTYWIDGET_H_
#define GLOBALPROPERTYWIDGET_H_

#include <QtGui/QWidget>

class GlobalPropertyView;
class QAbstractItemModel;
class QItemSelectionModel;
class QVBoxLayout;
class SliderWithEditInt;

class GlobalPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    GlobalPropertyWidget( QWidget* parent = 0 );
    virtual ~GlobalPropertyWidget();

    void setModel( QAbstractItemModel* model );

private:
    GlobalPropertyView* m_propertyView;

    QVBoxLayout* m_layout;

    SliderWithEditInt* m_sagittalSlider;
    SliderWithEditInt* m_coronalSlider;
    SliderWithEditInt* m_axialSlider;

public slots:
    void dataChanged();

private slots:
    void sliderChanged( int value, int id );

};

#endif /* DATASETPROPERTYWIDGET_H_ */
