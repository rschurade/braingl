/*
 * propertypath.h
 *
 * Created on: Mar 6, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYPATH_H_
#define PROPERTYPATH_H_

#include "property.h"

#include <QDir>

class PathWidgetWithLabel;

class PropertyPath : public Property
{
    Q_OBJECT

public:
    PropertyPath( QString name, QDir value );
    virtual ~PropertyPath();

private:

public slots:
    void setValue( QVariant value );
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( QDir value, int id );
};

#endif /* PROPERTYPATH_H_ */
