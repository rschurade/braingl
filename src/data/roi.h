/*
 * roi.h
 *
 * Created on: 03.02.2013
 * @author Ralph Schurade
 */

#ifndef ROI_H_
#define ROI_H_

#include "enums.h"
#include "properties/propertygroup.h"

#include <QMatrix4x4>
#include <QObject>

class ROI : public QObject
{
    Q_OBJECT

public:
    ROI( QString name );
    virtual ~ROI();

    PropertyGroup* properties();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode ) = 0;

    static int m_count;

protected:
    PropertyGroup m_properties;

public slots:
    void slotPropChanged();

signals:
    void signalPropChanged( int id );
};

#endif /* ROI_H_ */
