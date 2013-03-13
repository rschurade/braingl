/*
 * property.h
 *
 *  Created on: Jan 17, 2013
 *      Author: Ralph Schurade
 */
#ifndef PROPERTY_H_
#define PROPERTY_H_

#include "../enums.h"

#include <QString>
#include <QVariant>
#include <QObject>

#include <limits>

class QWidget;

class Property : public QObject
{
public:
    Property( QString name );
    virtual ~Property();

    QString getName();

    virtual QWidget* getWidget() = 0;
    virtual QVariant getValue() = 0;
    virtual void setValue( QVariant value ) = 0;

protected:
    QString m_name; // used for access

private:

};

#endif /* PROPERTY_H_ */
