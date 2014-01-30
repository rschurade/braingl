/*
 * propertyselection.h
 *
 *  Created on: Apr 3, 2013
 *      Author: schurade
 */

#ifndef PROPERTYSELECTION_H_
#define PROPERTYSELECTION_H_

#include "property.h"

#include "../../gui/widgets/controls/selectwithlabel.h"

#include <initializer_list>

class PropertySelection : public Property
{
    Q_OBJECT

public:
    PropertySelection( QString name, std::initializer_list<QString> options, int value = 0 );
    PropertySelection( QString name, std::vector<QString> options, int value = 0 );
    PropertySelection( QString name, QList<QString> options, int value = 0 );
    virtual ~PropertySelection();

    std::vector<QString> getOptions();
private:
    std::vector<QString>m_options;

public slots:
    void setValue( QVariant value );
    void setMin( QVariant value ) {};
    void setMax( QVariant value ) {};

private slots:
    void widgetChanged( int value );
};

#endif /* PROPERTYSELECTION_H_ */
