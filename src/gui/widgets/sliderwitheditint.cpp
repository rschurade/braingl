/*
 * sliderwithedit.cpp
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */
#include <QtGui/QSlider>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtCore/QDebug>

#include "sliderwitheditint.h"

SliderWithEditInt::SliderWithEditInt( QString name, QWidget* parent ) :
    QHBoxLayout( parent )
{
    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit();
    m_edit->setMaxLength( 3 );
    m_edit->setMaximumWidth( 35 );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    addWidget( new QLabel( name ) );
    addWidget( m_slider );
    addWidget( m_edit );
}

SliderWithEditInt::~SliderWithEditInt()
{

}

void SliderWithEditInt::sliderMoved( int value )
{
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
    emit( valueChanged( value ) );
}

void SliderWithEditInt::editEdited()
{
    QString text = m_edit->text();
    m_slider->setValue( text.toInt() );
    emit( valueChanged( text.toInt() ) );
    m_slider->repaint();
}

void SliderWithEditInt::setValue( int value )
{
    m_slider->setValue( value );
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
}

int SliderWithEditInt::getValue()
{
    return m_slider->value();
}

void SliderWithEditInt::setMin( int min )
{
    m_slider->setMinimum( min );
    m_slider->repaint();
}


void SliderWithEditInt::setMax( int max )
{
    m_slider->setMaximum( max );
    m_slider->repaint();
}
