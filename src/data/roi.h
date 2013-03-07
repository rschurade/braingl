/*
 * roi.h
 *
 *  Created on: 03.02.2013
 *      Author: Ralph Schurade
 */

#ifndef ROI_H_
#define ROI_H_

#include "enums.h"
#include "properties/roipropertygroup.h"

#include <QtGui/QMatrix4x4>
#include <QtCore/QObject>

class QAbstractItemModel;

class ROI : public QObject
{
public:
    ROI( QString name, QAbstractItemModel* globals );
    virtual ~ROI();

    ROIPropertyGroup* properties();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix ) = 0;

protected:
    ROIPropertyGroup m_properties;
    QAbstractItemModel* m_globals;
};

#endif /* ROI_H_ */
