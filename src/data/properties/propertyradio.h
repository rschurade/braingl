/*
 * propertyradio.h
 *
 *  Created on: 21.08.2014
 *      Author: Ralph
 */

#ifndef PROPERTYRADIO_H_
#define PROPERTYRADIO_H_

#include "property.h"

#include "../../gui/widgets/controls/radiogroup.h"


#include <QList>
#include <QString>

#include <initializer_list>


class PropertyRadio : public Property
{
    Q_OBJECT

public:
    PropertyRadio( QString name, std::initializer_list<QString> options, int value = 0 );
    PropertyRadio( QString name, std::vector<QString> options, int value = 0 );
    PropertyRadio( QString name, QList<QString> options, int value = 0 );
    virtual ~PropertyRadio();

    QList<QString> getOptions();

private:
    QList<QString>m_options;

public slots:
    void setValue( QVariant value );
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( int value );

};

#endif /* PROPERTYRADIO_H_ */
