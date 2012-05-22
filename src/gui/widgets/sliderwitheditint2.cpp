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

#include "sliderwitheditint2.h"

SliderWithEditInt2::SliderWithEditInt2( QWidget* parent ) :
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

    layout->addWidget( m_slider );
    layout->addWidget( m_edit );

    setLayout( layout );
}

SliderWithEditInt2::~SliderWithEditInt2()
{

}

void SliderWithEditInt2::sliderMoved( int value )
{
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
    emit( valueChanged( value ) );
}

void SliderWithEditInt2::editEdited()
{
    QString text = m_edit->text();
    m_slider->setValue( text.toInt() );
    emit( valueChanged( text.toInt() ) );
    m_slider->repaint();
}

void SliderWithEditInt2::setValue( int value )
{
    m_slider->setValue( value );
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
}

int SliderWithEditInt2::getValue()
{
    return m_slider->value();
}

void SliderWithEditInt2::setMin( int min )
{
    m_slider->setMinimum( min );
    m_slider->repaint();
}


void SliderWithEditInt2::setMax( int max )
{
    m_slider->setMaximum( max );
    m_slider->repaint();
}
