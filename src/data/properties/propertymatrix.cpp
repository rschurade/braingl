/*
 * propertymatrix.cpp
 *
 *  Created on: Oct 1, 2013
 *      Author: schurade
 */

#include "propertymatrix.h"

#include "../../gui/widgets/controls/matrixwidget.h"

PropertyMatrix::PropertyMatrix( QString name, QMatrix4x4 value ) :
    Property( name, value )
{
    MatrixWidget* widget = new MatrixWidget( name );
    widget->setValue( value );
    connect( widget, SIGNAL( valueChanged( int, QMatrix4x4 ) ), this, SLOT( widgetChanged( int, QMatrix4x4 ) ) );
    m_widget = widget;
}

PropertyMatrix::~PropertyMatrix()
{
}

void PropertyMatrix::setValue( QVariant value )
{
    m_value = value;
    dynamic_cast<MatrixWidget*>( m_widget )->setValue( value.value<QMatrix4x4>() );
}

void PropertyMatrix::widgetChanged( int id, QMatrix4x4 value )
{

}
