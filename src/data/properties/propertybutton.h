/*
 * propertybutton.h
 *
 *  Created on: Aug 20, 2013
 *      Author: schurade
 */

#ifndef PROPERTYBUTTON_H_
#define PROPERTYBUTTON_H_

#include "property.h"

class ButtonWithLabel;

class PropertyButton : public Property
{
    Q_OBJECT

public:
    PropertyButton( QString name );
    virtual ~PropertyButton();

    void setValue( QVariant value ) {};
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( int id );
};

#endif /* PROPERTYBUTTON_H_ */
