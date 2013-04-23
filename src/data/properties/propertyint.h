/*
 * propertyint.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYINT_H_
#define PROPERTYINT_H_

#include "property.h"

class SliderWithEditInt;
class QWidget;

class PropertyInt : public Property
{
    Q_OBJECT

public:
    PropertyInt( QString name, int value = 0, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max() );
    virtual ~PropertyInt();

    QWidget* getWidget();

    QVariant getValue();

private:
    int m_value;
    int m_min;
    int m_max;

    SliderWithEditInt* m_widget;

public slots:
    void setValue( QVariant value );
    void setMin( QVariant min );
    void setMax( QVariant max );

private slots:
    void widgetChanged( int value, int id );

signals:
    void valueChanged();
    void valueChanged( int );

};

#endif /* PROPERTYINT_H_ */
