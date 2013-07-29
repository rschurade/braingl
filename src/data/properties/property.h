/*
 * property.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <QString>
#include <QVariant>
#include <QObject>

#include <limits>

class QWidget;

class Property : public QObject
{
    Q_OBJECT

public:
    Property( QString name, QVariant value, QVariant min = 0, QVariant max = 0 );
    virtual ~Property();

    QString getName();

    virtual QWidget* getWidget();

    QVariant getValue();
    QVariant getMin();
    QVariant getMax();


    virtual void setValue( QVariant value ) = 0;
    virtual void setMin( QVariant value ) = 0;
    virtual void setMax( QVariant value ) = 0;

    void setPropertyTab( QString tab );
    QString getPropertyTab();

protected:
    QWidget* m_widget;

    QString m_name; // used for access
    QString m_propertyTab;

    QVariant m_value;
    QVariant m_min;
    QVariant m_max;

private:

signals:
    void valueChanged( QVariant );

};

#endif /* PROPERTY_H_ */
