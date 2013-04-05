/*
 * propertyselection.h
 *
 *  Created on: Apr 3, 2013
 *      Author: schurade
 */

#ifndef PROPERTYSELECTION_H_
#define PROPERTYSELECTION_H_

#include "property.h"

#include "../../gui/widgets/controls/selectwithlabel.h"

#include <initializer_list>

class PropertySelection : public Property
{
    Q_OBJECT

public:
    PropertySelection( QString name, std::initializer_list<QString> options, int value = 0 );
    PropertySelection( QString name, QVector<QString> options, int value = 0 );
    virtual ~PropertySelection();

    QWidget* getWidget();

    QVariant getValue();

private:
    int m_value;

    SelectWithLabel* m_widget;

public slots:
    void setValue( QVariant value );

private slots:
    void widgetChanged( int value );

signals:
    void valueChanged();
    void valueChanged( int );
};

#endif /* PROPERTYSELECTION_H_ */
