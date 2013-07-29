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



private:
    int determineDigits();

public slots:
    void setValue( QVariant value );
    void setMin( QVariant min );
    void setMax( QVariant max );

private slots:
    void widgetChanged( float value, int id );
};

#endif /* PROPERTYFLOAT_H_ */
