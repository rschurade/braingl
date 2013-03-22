/*
 * sliderwithedit.cpp
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */
#include "sliderwitheditint.h"

#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFontMetrics>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

SliderWithEditInt::SliderWithEditInt( QString name, int id, QWidget* parent ) :
    QFrame( parent )
{
    m_id = id;

    m_slider = new QSlider( this );
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit( this );
    m_edit->setMaxLength( 10 );
    m_edit->setMaximumWidth( 45 );
    m_edit->setAlignment( Qt::AlignCenter );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    m_button1 = new QPushButton( tr(" - "), this );
    QFontMetrics _metrics( m_button1->fontMetrics() );
    m_button1->setMaximumWidth( _metrics.boundingRect("  -  ").width() );
    m_button2 = new QPushButton( tr(" + "), this );
    m_button2->setMaximumWidth( _metrics.boundingRect("  +  ").width() );

    connect( m_button1, SIGNAL( clicked() ), this, SLOT( minusPressed() ) );
    connect( m_button2, SIGNAL( clicked() ), this, SLOT( plusPressed() ) );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( name, this );
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

    setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } "
                     "QLineEdit { font: 12px; max-height: 14px }"
                     "QSlider { max-height: 14px }"
                     "QLabel { font:  bold 12px; }" );
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
