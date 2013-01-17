/*
 * propertybool.h
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#ifndef PROPERTYBOOL_H_
#define PROPERTYBOOL_H_

#include "property.h"

class CheckboxWithLabel;
class QWidget;

class PropertyBool : public Property
{
    Q_OBJECT

public:
    PropertyBool( FN_PROPERTY name, QString label, bool value = false );
    virtual ~PropertyBool();

    QWidget* getWidget();

    QVariant getValue();

    void setValue( QVariant value );

private:
    int m_value;

    CheckboxWithLabel* m_widget;

private slots:
    void widgetChanged( int value, int id );

signals:
    void valueChanged();

};

#endif /* PROPERTYINT_H_ */
