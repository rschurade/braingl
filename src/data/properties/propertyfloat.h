/*
 * propertyfloat.h
 *
 *  Created on: Jan 17, 2013
 *      Author: Ralph Schurade
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

    void setValue( QVariant value );
    void setMin( float min );
    void setMax( float max );

private:
    float m_value;
    float m_min;
    float m_max;

    SliderWithEdit* m_widget;

private slots:
    void widgetChanged( float value, int id );

signals:
    void valueChanged();
};

#endif /* PROPERTYFLOAT_H_ */
