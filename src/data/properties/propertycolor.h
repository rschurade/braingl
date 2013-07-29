/*
 * propertycolor.h
 *
 * Created on: Feb 4, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYCOLOR_H_
#define PROPERTYCOLOR_H_

#include "property.h"

#include <QColor>

class ColorWidgetWithLabel;

class PropertyColor : public Property
{
    Q_OBJECT

public:
    PropertyColor( QString name, QColor value = QColor( 0, 0, 0 ) );
    virtual ~PropertyColor();

private:

public slots:
    void setValue( QVariant value );
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( QColor value, int id );

};

#endif /* PROPERTYCOLOR_H_ */
