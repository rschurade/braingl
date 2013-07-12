/*
 * propertygroup.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYGROUP_H_
#define PROPERTYGROUP_H_

#include "property.h"
#include "propertybool.h"
#include "propertycolor.h"
#include "propertyint.h"
#include "propertyfloat.h"
#include "propertypath.h"
#include "propertystring.h"
#include "propertyselection.h"

#include <QColor>
#include <QObject>
#include <QString>
#include <QVariant>

#include <initializer_list>

class PropertyGroup : public QObject
{
    Q_OBJECT

public:
    PropertyGroup();
    virtual ~PropertyGroup();

    bool contains( Fn::Property name ) const;
    QVariant get( Fn::Property name ) const;

    bool set( Fn::Property name, QVariant value );
    bool set2( Fn::Property name, QVariant value );
    bool set( Fn::Property name, bool value, QString tab = "none" );
    bool set( Fn::Property name, int value, QString tab = "none" );
    bool set( Fn::Property name, int value, int min, int max, QString tab = "none" );
    bool set( Fn::Property name, float value, QString tab = "none" );
    bool set( Fn::Property name, float value, float min, float max, QString tab = "none" );
    bool set( Fn::Property name, QString value, QString tab = "none" );
    bool set( Fn::Property name, const char* value, QString tab = "none" );
    bool set( Fn::Property name, QColor value, QString tab = "none" );
    bool set( Fn::Property name, QDir value, QString tab = "none" );
    bool set( Fn::Property name, std::initializer_list<QString> options, int value = 0, QString tab = "none" );
    bool set( Fn::Property name, QVector<QString> options, int value = 0, QString tab = "none" );

    bool setMinF( Fn::Property name,  float value );
    bool setMaxF( Fn::Property name,  float value );
    bool setMinI( Fn::Property name,  int value );
    bool setMaxI( Fn::Property name,  int value );

    int size() const;

    QWidget* getWidget( Fn::Property name );

    Property* getProperty( Fn::Property name );
    Property* getNthProperty( int n );

private:
    QVector<QPair<Fn::Property, Property*> >m_properties;

public slots:
    void slotPropChanged();

signals:
    void signalPropChanged();
};

#endif /* PROPERTYGROUP_H_ */
