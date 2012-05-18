/*
 * globalpropertywidget.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef GLOBALPROPERTYWIDGET_H_
#define GLOBALPROPERTYWIDGET_H_

#include <QtGui/QDockWidget>

class GlobalPropertyView;
class QAbstractItemModel;
class QItemSelectionModel;

class GlobalPropertyWidget : public QDockWidget
{
    Q_OBJECT

public:
    GlobalPropertyWidget( QString name, QWidget* parent = 0 );
    virtual ~GlobalPropertyWidget();

    void setModel( QAbstractItemModel* model );

private:
    GlobalPropertyView* m_propertyView;
};

#endif /* DATASETPROPERTYWIDGET_H_ */
