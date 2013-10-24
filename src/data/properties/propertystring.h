/*
 * propertystring.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYSTRING_H_
#define PROPERTYSTRING_H_

#include "property.h"

#include <QString>

class EditWithLabel;
class QWidget;

class PropertyString : public Property
{
    Q_OBJECT

public:
    PropertyString( QString name, QString value = ""  );
    virtual ~PropertyString();

private:

public slots:
    void setValue( QVariant value );
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( QString value, int id );

};

#endif /* PROPERTYINT_H_ */
