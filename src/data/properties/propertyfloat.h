/*
 * propertyfloat.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */

#ifndef PROPERTYFLOAT_H_
#define PROPERTYFLOAT_H_

#include "property.h"

class SliderWithEdit;
class QWidget;

class PropertyFloat :public Property
{
    Q_OBJECT

public:
    PropertyFloat( QString name,
                     float value = 0.0,
                     float min = std::numeric_limits<float>::min(),
                     float max = std::numeric_limits<float>::max() );
    virtual ~PropertyFloat();

    QWidget* getWidget();

    QVariant getValue();

private:
    int determineDigits();

    float m_value;
    float m_min;
    float m_max;

    SliderWithEdit* m_widget;

public slots:
    void setValue( QVariant value );
    void setMin( QVariant min );
    void setMax( QVariant max );

private slots:
    void widgetChanged( float value, int id );

signals:
    void valueChanged();
    void valueChanged( float value );
};

#endif /* PROPERTYFLOAT_H_ */
