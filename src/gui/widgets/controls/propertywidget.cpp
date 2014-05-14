/*
 * propertywidget.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: schurade
 */

#include "propertywidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QDebug>


PropertyWidget::PropertyWidget( QString label, int id, QWidget* parent ) :
    QFrame( parent ),
    m_id( id ),
    m_controlWidget( 0 ),
    m_showLabel( true ),
    m_labelPosition( top )
{
    m_label = new QLabel( label, this );
}

PropertyWidget::~PropertyWidget()
{
    delete m_controlWidget;
    delete m_label;
}

void PropertyWidget::showLabel( bool show )
{
    m_showLabel = show;
}

void PropertyWidget::setLabelPosition( Position position )
{
    m_labelPosition = position;
}

void PropertyWidget::setup()
{
    QHBoxLayout* controlLayout = new QHBoxLayout();
    QHBoxLayout* labelLayout = new QHBoxLayout();
    QVBoxLayout* vLayout = new QVBoxLayout();

    switch ( m_labelPosition )
    {
        case top:
            labelLayout->addWidget( m_label );
            labelLayout->addStretch();
            controlLayout->addWidget( m_controlWidget );
            controlLayout->addStretch();
            if ( m_showLabel )
            {
                vLayout->addLayout( labelLayout );
            }
            vLayout->addLayout( controlLayout );
            break;
        case bottom:
            controlLayout->addWidget( m_controlWidget );
            controlLayout->addStretch();
            labelLayout->addWidget( m_label );
            labelLayout->addStretch();
            vLayout->addLayout( controlLayout );
            if ( m_showLabel )
            {
                vLayout->addLayout( labelLayout );
            }
            break;
        case right:
            controlLayout->addWidget( m_controlWidget );
            if ( m_showLabel )
            {
                controlLayout->addStretch();
                controlLayout->addWidget( m_label );
            }
            vLayout->addLayout( controlLayout );
            break;
        case left:
        default:
            if ( m_showLabel )
            {
                controlLayout->addWidget( m_label );
                controlLayout->addStretch();
            }
            controlLayout->addWidget( m_controlWidget );
            vLayout->addLayout( controlLayout );
            /* NO BREAK */
            break;
    }

    controlLayout->setContentsMargins( 1, 1, 1, 1 );
    labelLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setContentsMargins( 0, 0, 0, 0 );



    setLayout( vLayout );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px }" );

}
