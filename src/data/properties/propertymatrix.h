/*
 * propertymatrix.h
 *
 *  Created on: Oct 1, 2013
 *      Author: schurade
 */

#ifndef PROPERTYMATRIX_H_
#define PROPERTYMATRIX_H_

#include "property.h"

#include <QMatrix4x4>

class PropertyMatrix  : public Property
{
    Q_OBJECT

public:
    PropertyMatrix( QString name, QMatrix4x4 value );
    virtual ~PropertyMatrix();

public slots:
    void setValue( QVariant value );
    void setValue( int row, int column, float val );
    void widgetChanged( int id, QMatrix4x4 value );

};

#endif /* PROPERTYMATRIX_H_ */
