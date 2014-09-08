/*
 * colorwidgetwithlabel.cpp
 *
 * Created on: Feb 4, 2013
 * @author Ralph Schurade
 */
#include "colorwidgetwithlabel.h"

#include <QColorDialog>
#include <QDebug>
#include <QVariant>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

ColorWidgetWithLabel::ColorWidgetWithLabel( QString label, int id, QWidget* parent ) :
    QFrame( parent ),
    m_id( id ),
    m_value( QColor( 255, 255, 255 ) )
{
    m_button = new QPushButton( QString( "set color" ), this );
    m_label = new QLabel( label, this );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    if ( label != "" )
    {
        hLayout->addWidget( m_label );
        hLayout->addStretch();
    }
    hLayout->addWidget( m_button );
    vLayout->addLayout( hLayout );

    hLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setContentsMargins( 0, 0, 0, 0 );

    setLayout( vLayout );

    connect( m_button, SIGNAL( clicked() ), this, SLOT( slotButton() ) );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    //m_button->setStyleSheet("* { background-color: " + m_value.name() + "; }");

    QPalette pal = m_button->palette();
    pal.setColor( QPalette::Button, m_value );
    m_button->setAutoFillBackground(true);
    m_button->setPalette( pal );
    m_button->update();

    setStyleSheet( "QLabel { font:  bold 12px } "
                   "QPushButton { font:  bold 12px; max-height: 16px; } ");
}

ColorWidgetWithLabel::~ColorWidgetWithLabel()
{
}

void ColorWidgetWithLabel::setValue( QColor value )
{
    m_value = value;
    //m_button->setStyleSheet("* { background-color: " + m_value.name() + " }");
    QPalette pal = m_button->palette();
    pal.setColor( QPalette::Button, value );
    m_button->setAutoFillBackground(true);
    m_button->setPalette( pal );
    m_button->update();
}

QColor ColorWidgetWithLabel::getValue()
{
    return m_value;
}

void ColorWidgetWithLabel::slotButton()
{
    QColorDialog* cd = new QColorDialog( m_value );
    cd->open();
    connect( cd, SIGNAL( colorSelected( QColor ) ), this, SLOT( currentColorChanged( QColor ) ) );
}

void ColorWidgetWithLabel::currentColorChanged( QColor color )
{
    m_value = color;
    //m_button->setStyleSheet("* { background-color: " + m_value.name() + "; }");
    QPalette pal = m_button->palette();
    pal.setColor( QPalette::Button, color );
    m_button->setAutoFillBackground(true);
    m_button->setPalette( pal );
    m_button->update();
    emit( colorChanged( m_value, m_id ) );
}
