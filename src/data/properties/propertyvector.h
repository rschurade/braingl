/*
 * propertymatrix.h
 *
 *  Created on: Oct 1, 2013
 *      Author: schurade
 */

#ifndef PROPERTYVECTOR_H_
#define PROPERTYVECTOR_H_

#include "property.h"

#include <QVector3D>

class PropertyVector  : public Property
{
    Q_OBJECT

public:
    PropertyVector( QString name, QVector3D value );
    virtual ~PropertyVector();

public slots:
    void setValue( QVariant value );
    void setValue( int column, float val );
    void widgetChanged( int id, QVector3D value );

};

#endif /* PROPERTYMATRIX_H_ */
