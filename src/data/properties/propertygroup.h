/*
 * propertygroup.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYGROUP_H_
#define PROPERTYGROUP_H_

#include "../enums.h"

#include "property.h"

#include <QDir>
#include <QColor>
#include <QMatrix4x4>
#include <QVector3D>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

#include <initializer_list>

class PropertyGroup : public QObject
{
    Q_OBJECT

public:
    PropertyGroup();
    PropertyGroup( const PropertyGroup& pg );
    virtual ~PropertyGroup();

    PropertyGroup& operator=( const PropertyGroup& pg );

    bool contains( Fn::Property name ) const;
    QVariant get( Fn::Property name ) const;
    bool set( Fn::Property name, QVariant value );

    bool createBool( Fn::Property name, bool value, QString tab = "none" );
    bool createInt( Fn::Property name, int value, QString tab = "none" );
    bool createInt( Fn::Property name, int value, int min, int max, QString tab = "none" );
    bool createFloat( Fn::Property name, float value, QString tab = "none" );
    bool createFloat( Fn::Property name, float value, float min, float max, QString tab = "none" );
    bool createString( Fn::Property name, QString value, QString tab = "none" );
    bool createCharString( Fn::Property name, const char* value, QString tab = "none" );
    bool createColor( Fn::Property name, QColor value, QString tab = "none" );
    bool createDir( Fn::Property name, QDir value, QString tab = "none" );
    bool createList( Fn::Property name, std::initializer_list<QString> options, int value = 0, QString tab = "none" );
    bool createList( Fn::Property name, QList<QString> options, int value = 0, QString tab = "none" );
    bool createList( Fn::Property name, std::vector<QString> options, int value = 0, QString tab = "none" );
    bool createButton( Fn::Property name, QString tab = "none" );
    bool createMatrix( Fn::Property name, QMatrix4x4 value, QString tab = "none" );
    bool createVector( Fn::Property name, QVector3D value, QString tab = "none" );
    bool createText( Fn::Property name, QString value, QString tab = "none" );

    bool setMin( Fn::Property name,  QVariant value );
    bool setMax( Fn::Property name,  QVariant value );

    int size() const;

    QWidget* getWidget( Fn::Property name );

    Property* getProperty( Fn::Property name );
    Property* getNthProperty( int n );
    QPair<Fn::Property, Property*>getNthPropertyPair( int n ) const;

    QList<QVariant>getState();
    void setState( QList<QVariant> state );

private:
    std::vector<QPair<Fn::Property, Property*> >m_properties;

public slots:
    void slotPropChanged();

signals:
    void signalPropChanged();
    void signalSetProp( int id );
};

#endif /* PROPERTYGROUP_H_ */
