/*
 * roipropertygroup.h
 *
 * Created on: Feb 02, 2013
 * @author Ralph Schurade
 */

#ifndef ROIPROPERTYGROUP_H_
#define ROIPROPERTYGROUP_H_

#include "property.h"
#include "propertybool.h"
#include "propertycolor.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertystring.h"
#include "propertyselection.h"

#include <QObject>
#include <QString>
#include <QVariant>

class ROIPropertyGroup : public QObject
{
    Q_OBJECT

public:
    ROIPropertyGroup();
    virtual ~ROIPropertyGroup();

    bool contains( Fn::ROI name ) const;
    QVariant get( Fn::ROI name ) const;

    bool set( Fn::ROI name, bool value, bool visible = false );
    bool set( Fn::ROI name, int value, bool visible = false );
    bool set( Fn::ROI name, int value, int min, int max, bool visible = false );
    bool set( Fn::ROI name, float value, bool visible = false );
    bool set( Fn::ROI name, float value, float min, float max, bool visible = false );
    bool set( Fn::ROI name, QString value, bool visible = false );
    bool set( Fn::ROI name, const char* value, bool visible = false );
    bool set( Fn::ROI name, QVariant value );
    bool set( Fn::ROI name, QColor value, bool visible = false );
    bool set( Fn::ROI name, std::initializer_list<QString>options, int value = 0, bool visible = false );

    int size() const;

    QList<Fn::ROI> getVisible();
    QWidget* getWidget( Fn::ROI name );
    Property* getProperty( Fn::ROI name );

private:
    QHash<int, Property*> m_properties;
    QList<Fn::ROI>m_visible;

public slots:
    void slotPropChanged();

signals:
    void signalPropChanged( int );
};

#endif /* ROIPROPERTYGROUP_H_ */
