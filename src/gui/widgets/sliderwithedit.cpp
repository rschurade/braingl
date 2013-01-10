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
#include <QtGui/QHBoxLayout>

#include "sliderwithedit.h"

SliderWithEdit::SliderWithEdit( QString name, int id, QWidget* parent ) :
    QWidget( parent )
{
    m_id = id;

    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit();
    m_edit->setMaxLength( 6 );
    m_edit->setMaximumWidth( 55 );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    QHBoxLayout* layout = new QHBoxLayout();

    m_label = new QLabel( name );

    layout->addWidget( m_label, 25 );
    layout->addWidget( m_slider, 60 );
    layout->addWidget( m_edit, 15 );

    setLayout( layout );
}

SliderWithEdit::~SliderWithEdit()
{
    delete m_slider;
    delete m_label;
    delete m_edit;
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
    emit( valueChanged( ( static_cast<float>( value ) / 100. ), m_id  ) );
}

void SliderWithEdit::editEdited()
{
    QString text = m_edit->text();
    m_slider->setValue( static_cast<int>( text.toFloat() * 100 ) );
    emit( valueChanged( text.toFloat(), m_id ) );
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
