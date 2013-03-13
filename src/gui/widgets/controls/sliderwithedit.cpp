/*
 * sliderwithedit.cpp
 *
 *  Created on: 15.05.2012
 *      Author: Ralph Schurade
 */
#include "sliderwithedit.h"

#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

SliderWithEdit::SliderWithEdit( QString name, int id, QWidget* parent ) :
    QFrame( parent )
{
    m_id = id;

    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit();
    m_edit->setMaxLength( 10 );
    m_edit->setMaximumWidth( 55 );
    m_edit->setAlignment( Qt::AlignCenter );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( name );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget( m_slider, 70 );
    hLayout2->addWidget( m_edit, 30 );

    if ( name != "" )
    {
        vLayout->addLayout( hLayout );
    }
    vLayout->addLayout( hLayout2 );

    hLayout->setContentsMargins( 0,0,0,0 );
    hLayout2->setContentsMargins( 0,0,0,0 );
    vLayout->setContentsMargins( 1,1,1,1 );


    setLayout( vLayout );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px; margin-bottom: -1px }"
                     "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; margin-top: -1px } "
                     "QSlider { max-height: 14px; margin-top: -1px }"
                     "QLineEdit { font: 12px; max-height: 14px; margin-top: -1px }" );
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
