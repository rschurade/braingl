/*
 * sliderwithedit.cpp
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */
#include "sliderwitheditint.h"

#include "myslider.h"

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

    m_slider = new MySlider( this );
    m_slider->setOrientation( Qt::Horizontal );

    m_edit = new QLineEdit( this );
    m_edit->setMaxLength( 10 );
    m_edit->setMaximumWidth( 60 );
    m_edit->setAlignment( Qt::AlignCenter );
    m_edit->setText( "0" );

    connect( m_slider, SIGNAL( sliderMoved( int ) ), this, SLOT( sliderMoved( int ) ) );
    connect( m_edit, SIGNAL( editingFinished() ), this, SLOT( editEdited() ) );

    m_button1 = new QPushButton( tr(" - "), this );
    QFontMetrics _metrics( m_button1->fontMetrics() );
    m_button1->setMaximumWidth( _metrics.boundingRect("  -  ").width() );
    m_button2 = new QPushButton( tr(" + "), this );
    m_button2->setMaximumWidth( _metrics.boundingRect("  +  ").width() );

    connect( m_button1, SIGNAL( clicked() ), this, SLOT( decrement() ) );
    connect( m_button2, SIGNAL( clicked() ), this, SLOT( increment() ) );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QPushButton* hideMinMax = new QPushButton( " - " );
    QPushButton* showMinMax = new QPushButton( "+" );

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( name );
    hLayout->addWidget( hideMinMax );
    hideMinMax->hide();
    hLayout->addWidget( showMinMax );
    m_label = new QLabel( name, this );
    hLayout->addWidget( m_label );
    hLayout->addStretch();
    hLayout->addWidget( m_edit );

    QHBoxLayout* minMaxLayout = new QHBoxLayout();
    QLabel* lab1 = new QLabel( "min" );
    m_edit1 = new QLineEdit();
    QLabel* lab2 = new QLabel( "max" );
    m_edit2 = new QLineEdit();
    minMaxLayout->addWidget( lab1 );
    minMaxLayout->addWidget( m_edit1 );
    minMaxLayout->addWidget( lab2 );
    minMaxLayout->addWidget( m_edit2 );

    connect( m_edit1, SIGNAL( editingFinished() ), this, SLOT( minEdited() ) );
    connect( m_edit2, SIGNAL( editingFinished() ), this, SLOT( maxEdited() ) );

    lab1->hide();
    lab2->hide();
    m_edit1->hide();
    m_edit2->hide();
    connect( hideMinMax, SIGNAL( clicked() ), lab1, SLOT( hide() ) );
    connect( showMinMax, SIGNAL( clicked() ), lab1, SLOT( show() ) );
    connect( hideMinMax, SIGNAL( clicked() ), lab2, SLOT( hide() ) );
    connect( showMinMax, SIGNAL( clicked() ), lab2, SLOT( show() ) );
    connect( hideMinMax, SIGNAL( clicked() ), m_edit1, SLOT( hide() ) );
    connect( showMinMax, SIGNAL( clicked() ), m_edit1, SLOT( show() ) );
    connect( hideMinMax, SIGNAL( clicked() ), m_edit2, SLOT( hide() ) );
    connect( showMinMax, SIGNAL( clicked() ), m_edit2, SLOT( show() ) );

    connect( showMinMax, SIGNAL( clicked() ), showMinMax, SLOT( hide() ) );
    connect( showMinMax, SIGNAL( clicked() ), hideMinMax, SLOT( show() ) );
    connect( hideMinMax, SIGNAL( clicked() ), hideMinMax, SLOT( hide() ) );
    connect( hideMinMax, SIGNAL( clicked() ), showMinMax, SLOT( show() ) );

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget( m_button1 );
    hLayout2->addWidget( m_slider );
    hLayout2->addWidget( m_button2 );

    vLayout->addLayout( hLayout );
    vLayout->addLayout( minMaxLayout );
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
    m_slider->setValue( value );
    if ( value == 0 )
    {
        m_edit->setText( "0" );
    }
    else
    {
        m_edit->setText( QString::number( value ) );
    }
    emit( valueChanged( value, m_id ) );
    m_slider->repaint();
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
    if ( value == 0 )
    {
        m_edit->setText( "0" );
    }
    else
    {
        m_edit->setText( QString::number( value ) );
    }
    m_slider->repaint();
}

int SliderWithEditInt::getValue()
{
    return m_slider->value();
}

void SliderWithEditInt::setMin( int min )
{
    m_slider->setMinimum( min );
    m_edit1->setText( QString::number( min ) );
    if ( m_slider->value() < min )
    {
        m_slider->setValue( min );
        m_edit->setText( QString::number( min ) );
    }
    m_slider->repaint();
}


void SliderWithEditInt::setMax( int max )
{
    m_slider->setMaximum( max );
    m_edit2->setText( QString::number( max ) );
    if ( m_slider->value() > max )
    {
        m_slider->setValue( max );
        m_edit->setText( QString::number( max ) );
    }
    m_slider->repaint();
}

void SliderWithEditInt::setStep( int step )
{
    m_slider->setSingleStep( step );
}

void SliderWithEditInt::decrement()
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

void SliderWithEditInt::increment()
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

void SliderWithEditInt::minEdited()
{
    QString text = m_edit1->text();
    emit( minChanged( text.toInt() ) );
}

void SliderWithEditInt::maxEdited()
{
    QString text = m_edit2->text();
    emit( maxChanged( text.toInt() ) );
}

