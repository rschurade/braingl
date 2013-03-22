/*
 * roi.h
 *
 * Created on: 03.02.2013
 * @author Ralph Schurade
 */

#ifndef ROI_H_
#define ROI_H_

#include "enums.h"
#include "properties/roipropertygroup.h"

#include <QMatrix4x4>
#include <QObject>

class ROI : public QObject
{
public:
    ROI( QString name );
    virtual ~ROI();

    ROIPropertyGroup* properties();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix ) = 0;

protected:
    ROIPropertyGroup m_properties;
};

#endif /* ROI_H_ */
