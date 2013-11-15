/*
 * propertytext.h
 *
 * Created on: Nov 15, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYTEXT_H_
#define PROPERTYTEXT_H_

#include "property.h"

#include <QString>

class TextEditWithLabel;
class QWidget;

class PropertyText : public Property
{
    Q_OBJECT

public:
    PropertyText( QString name, QString value = ""  );
    virtual ~PropertyText();

private:

public slots:
    void setValue( QVariant value );
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( QString value, int id );

};

#endif /* PROPERTYTEXT_H_ */
