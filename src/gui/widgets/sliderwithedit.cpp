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

#include "sliderwithedit.h"

SliderWithEdit::SliderWithEdit( QString name, QWidget* parent ) :
    QHBoxLayout( parent )
{
    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit();
    m_edit->setMaxLength( 6 );
    m_edit->setMaximumWidth( 50 );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( m_edit, SIGNAL( textEdited( QString ) ), this, SLOT( editChanged( QString ) ) );

    addWidget( new QLabel( name ) );
    addWidget( m_slider );
    addWidget( m_edit );
}

SliderWithEdit::~SliderWithEdit()
{

}

void SliderWithEdit::sliderChanged( int value )
{
    m_edit->setText( QString::number( static_cast<double>( value ) / 100., 'f', 2 ) );
}

void SliderWithEdit::sliderMoved( int value )
{
    m_edit->setText( QString::number( static_cast<float>( value ) / 100., 'f', 2 ) );
    emit( valueChanged( static_cast<float>( value ) / 100.  ) );
}

void SliderWithEdit::editChanged( QString text )
{
    m_slider->setValue( static_cast<int>( text.toFloat() * 100 ) );
    emit( valueChanged( static_cast<float>( m_slider->value() ) / 100. ) );
}

void SliderWithEdit::setValue( float value )
{
    m_slider->setValue( value * 100 );
}

float SliderWithEdit::getValue()
{
    return static_cast<float>( m_slider->value() ) / 100.;
}

void SliderWithEdit::setMin( float min )
{
    m_slider->setMinimum( min * 100 );
}


void SliderWithEdit::setMax( float max )
{
    m_slider->setMaximum( max * 100 );
}
