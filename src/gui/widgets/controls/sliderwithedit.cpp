/*
 * sliderwithedit.cpp
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */
#include "sliderwithedit.h"

#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define SLIDERMULT 1000.f

SliderWithEdit::SliderWithEdit( QString name,  Fn::Position editPos, int id, QWidget* parent ) :
    QFrame( parent ),
    m_id( id ),
    m_digits( 2 )
{
    m_slider = new QSlider();
    m_slider->setOrientation( Qt::Horizontal );
    m_slider->setValue( -1 );

    m_edit = new QLineEdit();
    m_edit->setMaxLength( 10 );
    m_edit->setMaximumWidth( 100 );
    m_edit->setAlignment( Qt::AlignCenter );
    m_edit->setText( "0.00" );


    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( name );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    if ( editPos == Fn::Position::NORTH_EAST )
    {
        hLayout->addWidget( m_edit );
    }

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget( m_slider );
    if ( editPos == Fn::Position::EAST )
    {
        hLayout2->addWidget( m_edit );
    }

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
    if ( value == 0 )
    {
        m_edit->setText( "0.00" );
    }
    else
    {
        m_edit->setText( QString::number( static_cast<double>( value ) / SLIDERMULT, 'f', m_digits ) );
    }
    m_slider->repaint();
}

void SliderWithEdit::sliderMoved( int value )
{
    m_edit->setText( QString::number( static_cast<float>( value ) / SLIDERMULT, 'f', m_digits ) );
    m_slider->repaint();
    emit( valueChanged( ( static_cast<float>( value ) / SLIDERMULT ), m_id  ) );
}

void SliderWithEdit::editEdited()
{
    QString text = m_edit->text();
    m_slider->setValue( static_cast<int>( text.toFloat() * SLIDERMULT ) );
    emit( valueChanged( text.toFloat(), m_id ) );
    m_slider->repaint();
}

void SliderWithEdit::setValue( float value )
{
    m_slider->setValue( value * SLIDERMULT );
    m_slider->repaint();
}

float SliderWithEdit::getValue()
{
    return static_cast<float>( m_slider->value() ) / SLIDERMULT;
}

void SliderWithEdit::setMin( float min )
{
    m_slider->setMinimum( min * SLIDERMULT );
    m_slider->repaint();
}


void SliderWithEdit::setMax( float max )
{
    m_slider->setMaximum( max * SLIDERMULT );
    m_slider->repaint();
}

void SliderWithEdit::setDigits( int digits )
{
    m_digits = digits;
}
