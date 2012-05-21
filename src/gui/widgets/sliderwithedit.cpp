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
    m_edit->setMaximumWidth( 55 );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

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
    m_slider->repaint();
}

void SliderWithEdit::sliderMoved( int value )
{
    m_edit->setText( QString::number( static_cast<float>( value ) / 100., 'f', 2 ) );
    m_slider->repaint();
    emit( valueChanged( static_cast<float>( value ) / 100.  ) );
}

void SliderWithEdit::editEdited()
{
    QString text = m_edit->text();
    m_slider->setValue( static_cast<int>( text.toFloat() * 100 ) );
    emit( valueChanged( text.toFloat() ) );
    m_slider->repaint();
}

void SliderWithEdit::setValue( float value )
{
    m_slider->setValue( value * 100 );
    m_slider->repaint();
}

float SliderWithEdit::getValue()
{
    return static_cast<float>( m_slider->value() ) / 100.;
}

void SliderWithEdit::setMin( float min )
{
    m_slider->setMinimum( min * 100 );
    m_slider->repaint();
}


void SliderWithEdit::setMax( float max )
{
    m_slider->setMaximum( max * 100 );
    m_slider->repaint();
}
