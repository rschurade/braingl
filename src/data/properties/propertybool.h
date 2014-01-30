/*
 * propertybool.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
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
    PropertyBool( QString name, bool value = false );
    virtual ~PropertyBool();

private:

public slots:
    void setValue( QVariant value );
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( int value, int id );
};

#endif /* PROPERTYINT_H_ */
