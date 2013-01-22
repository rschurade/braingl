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
#include <QtGui/QVBoxLayout>

#include "sliderwitheditint.h"

SliderWithEditInt::SliderWithEditInt( QString name, int id, QWidget* parent ) :
    QFrame( parent )
{
    m_id = id;

    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit();
    m_edit->setMaxLength( 4 );
    m_edit->setMaximumWidth( 45 );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    m_button1 = new QPushButton( tr(" - ") );
    QFontMetrics _metrics( m_button1->fontMetrics() );
    m_button1->setMaximumWidth( _metrics.boundingRect("  -  ").width() );
    m_button2 = new QPushButton( tr(" + ") );
    m_button2->setMaximumWidth( _metrics.boundingRect("  +  ").width() );

    connect( m_button1, SIGNAL( clicked() ), this, SLOT( minusPressed() ) );
    connect( m_button2, SIGNAL( clicked() ), this, SLOT( plusPressed() ) );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( name );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget( m_button1 );
    hLayout2->addWidget( m_slider, 55 );
    hLayout2->addWidget( m_button2 );
    hLayout2->addWidget( m_edit, 25 );


    vLayout->addLayout( hLayout );
    vLayout->addLayout( hLayout2 );

    hLayout->setContentsMargins( 0,0,0,0 );
    hLayout2->setContentsMargins( 0,0,0,0 );
    vLayout->setContentsMargins( 1,1,1,1 );

    setLayout( vLayout );

    setFrameStyle( QFrame::Panel | QFrame::Raised );
}

SliderWithEditInt::~SliderWithEditInt()
{

}

void SliderWithEditInt::sliderMoved( int value )
{
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
    emit( valueChanged( value, m_id ) );
}

void SliderWithEditInt::editEdited()
{
    QString text = m_edit->text();
    m_slider->setValue( text.toInt() );
    emit( valueChanged( text.toInt(), m_id ) );
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

void SliderWithEditInt::setStep( int step )
{
    m_slider->setSingleStep( step );
}

void SliderWithEditInt::minusPressed()
{
    int current = m_slider->value();
    if ( current == m_slider->minimum() )
    {
        return;
    }
    int value = current - m_slider->singleStep();
    m_slider->setValue( value );
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
    emit( valueChanged( value, m_id ) );
}

void SliderWithEditInt::plusPressed()
{
    int current = m_slider->value();
    if ( current == m_slider->maximum() )
    {
        return;
    }
    int value = current + m_slider->singleStep();
    m_slider->setValue( value );
    m_edit->setText( QString::number( value ) );
    m_slider->repaint();
    emit( valueChanged( value, m_id ) );

}
