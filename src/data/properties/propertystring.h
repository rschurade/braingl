/*
 * propertystring.h
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#ifndef PROPERTYSTRING_H_
#define PROPERTYSTRING_H_

#include <QtCore/QString>

#include "property.h"

class EditWithLabel;
class QWidget;

class PropertyString : public Property
{
    Q_OBJECT

public:
    PropertyString( FN_PROPERTY name, QString value = ""  );
    virtual ~PropertyString();

    QWidget* getWidget();

    QVariant getValue();

    void setValue( QVariant value );

private:
    QString m_value;

    EditWithLabel* m_widget;

private slots:
    void widgetChanged( QString value, int id );

signals:
    void valueChanged();

};

#endif /* PROPERTYINT_H_ */
