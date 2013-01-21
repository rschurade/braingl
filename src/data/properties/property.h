/*
 * property.h
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include <QtCore/QString>
#include <QtCore/QVariant>

#include "../enums.h"

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <limits>

#include <QtCore/QObject>

class QWidget;

class Property : public QObject
{
public:
    Property( FN_PROPERTY name );
    virtual ~Property();

    FN_PROPERTY getName();
    QString getLabel();

    virtual QWidget* getWidget() = 0;
    virtual QVariant getValue() = 0;
    virtual void setValue( QVariant value ) = 0;

    static QString getNameAsString( FN_PROPERTY name );

protected:
    FN_PROPERTY m_name; // used for access
    QString m_label; // used for label in the dataset property view

private:

};

#endif /* PROPERTY_H_ */
