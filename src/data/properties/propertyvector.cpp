/*
 * propertymatrix.cpp
 *
 *  Created on: Oct 1, 2013
 *      Author: schurade
 */

#include "propertyvector.h"

#include "../../gui/widgets/controls/vectorwidget.h"

PropertyVector::PropertyVector( QString name, QVector3D value ) :
    Property( name, value )
{
    VectorWidget* widget = new VectorWidget( name );
    widget->setValue( value );
    connect( widget, SIGNAL( valueChanged( int, QVector3D ) ), this, SLOT( widgetChanged( int, QVector3D ) ) );
    m_widget = widget;
}

PropertyVector::~PropertyVector()
{
}

void PropertyVector::setValue( QVariant value )
{
    m_value = value;
    dynamic_cast<VectorWidget*>( m_widget )->setValue( value.value<QVector3D>() );
}

void PropertyVector::widgetChanged( int id, QVector3D value )
{
    m_value = value;
    emit( valueChanged( value ) );
}

void PropertyVector::setValue( int column, float val )
{
    QVector3D v = m_value.value<QVector3D>();
    switch ( column )
    {
        case 0:
            v.setX( val );
            break;
        case 1:
            v.setY( val );
            break;
        case 2:
            v.setZ( val );
            break;
        default:
            break;
    }
    m_value = v;
    dynamic_cast<VectorWidget*>( m_widget )->setValue( v );
}
