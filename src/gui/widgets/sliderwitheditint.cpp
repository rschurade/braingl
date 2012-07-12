/*
 * sliderwithedit.cpp
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */
#include <QtGui/QSlider>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QFontMetrics>
#include <QtCore/QDebug>
#include <QtGui/QHBoxLayout>

#include "sliderwitheditint.h"

SliderWithEditInt::SliderWithEditInt( QString name, QWidget* parent ) :
    QWidget( parent )
{
    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit();
    m_edit->setMaxLength( 3 );
    m_edit->setMaximumWidth( 35 );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    QHBoxLayout* layout = new QHBoxLayout();

    m_label = new QLabel( name );

    m_button1 = new QPushButton( tr(" - ") );
    QFontMetrics _metrics( m_button1->fontMetrics() );
    m_button1->setMaximumWidth( _metrics.boundingRect("  -  ").width() );
    m_button2 = new QPushButton( tr(" + ") );
    m_button2->setMaximumWidth( _metrics.boundingRect("  +  ").width() );

    connect( m_button1, SIGNAL( clicked() ), this, SLOT( minusPressed() ) );
    connect( m_button2, SIGNAL( clicked() ), this, SLOT( plusPressed() ) );

    layout->addWidget( m_label, 20 );
    layout->addWidget( m_button1 );
    layout->addWidget( m_slider, 60 );
    layout->addWidget( m_button2 );
    layout->addWidget( m_edit, 20 );

    setLayout( layout );
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

void SliderWithEditInt::minusPressed()
{
    int current = m_slider->value();
    if ( current == m_slider->minimum() )
    {
        return;
    }
    int value = current - 1;
    m_slider->setValue( value );
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
    emit( valueChanged( value ) );
}

void SliderWithEditInt::plusPressed()
{
    int current = m_slider->value();
    if ( current == m_slider->maximum() )
    {
        return;
    }
    int value = current + 1;
    m_slider->setValue( value );
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
    emit( valueChanged( value ) );

}
