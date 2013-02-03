/*
 * globalpropertygroup.h
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#ifndef GLOBALPROPERTYGROUP_H_
#define GLOBALPROPERTYGROUP_H_

#include "property.h"
#include "propertybool.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertystring.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

class GlobalPropertyGroup : public QObject
{
    Q_OBJECT

public:
    GlobalPropertyGroup();
    virtual ~GlobalPropertyGroup();

    bool contains( int name ) const;
    QVariant get( int name ) const;

    bool set( int name, bool value, bool visible = false );
    bool set( int name, int value, bool visible = false );
    bool set( int name, int value, int min, int max, bool visible = false );
    bool set( int name, float value, bool visible = false );
    bool set( int name, float value, float min, float max, bool visible = false );
    bool set( int name, QString value, bool visible = false );
    bool set( int name, const char* value, bool visible = false );
    bool set( int name, QVariant value );

    int size() const;

    QList<int> getVisible();
    QWidget* getWidget( int name );

private:
    QHash<int, Property*> m_properties;
    QList<int>m_visible;

private slots:
    void slotPropChanged();

signals:
    void signalPropChanged();
};

#endif /* GLOBALPROPERTYGROUP_H_ */
