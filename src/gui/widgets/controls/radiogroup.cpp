/*
 * radiogroup.cpp
 *
 *  Created on: 21.08.2014
 *      Author: Ralph
 */

#include "radiogroup.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>

RadioGroup::RadioGroup( QString label, QList<QString>options, int id, QWidget* parent ) :
    QFrame( parent ),
    m_id( id )
{
   QVBoxLayout* vLayout = new QVBoxLayout();

   QHBoxLayout* hLayout = new QHBoxLayout();
   m_label = new QLabel( label );
   hLayout->addWidget( m_label );
   hLayout->addStretch();

   vLayout->addLayout( hLayout );

   for ( auto i = 0; i < options.size(); ++i )
   {
       QHBoxLayout* hLayout2 = new QHBoxLayout();

       QRadioButton* rb = new QRadioButton( options[i], this );
       hLayout2->addWidget( rb );
       m_buttons.push_back( rb );
       connect( rb, SIGNAL( toggled( bool ) ), this, SLOT( indexChanged( bool ) ) );

       if ( i < options.size() - 1 )
       {
           QRadioButton* rb2 = new QRadioButton( options[++i], this );
           connect( rb2, SIGNAL( toggled( bool ) ), this, SLOT( indexChanged( bool ) ) );
           hLayout2->addWidget( rb2 );
           m_buttons.push_back( rb2 );
       }
       else
       {
           hLayout2->addStretch();
       }

       hLayout2->setContentsMargins( 0,0,0,0 );
       vLayout->addLayout( hLayout2 );
   }




   hLayout->setContentsMargins( 0,0,0,0 );
   vLayout->setContentsMargins( 1,1,1,1 );

   setLayout( vLayout );

   setFrameStyle( QFrame::Panel | QFrame::Raised );

   setStyleSheet( "QLabel { font:  bold 12px; margin-bottom: -1px }"
                    "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; margin-top: -1px } "
                    "QRadioButton { font:  bold 12px } "
                    "QSlider { max-height: 14px; margin-top: -1px }"
                    "QLineEdit { font: 12px; max-height: 14px; margin-top: -1px }" );
}
RadioGroup::~RadioGroup()
{
}

void RadioGroup::setCurrentIndex( int index )
{
    if ( index < m_buttons.size() )
    {
        m_buttons[index]->setChecked( true );
    }
}

int RadioGroup::getCurrentIndex()
{
    for ( auto i = 0; i < m_buttons.size(); ++i )
    {
        if ( m_buttons[i]->isChecked() )
        {
            return i;
        }
    }
    return -1;
}

QString RadioGroup::getSelectedItemString()
{
    for ( auto i = 0; i < m_buttons.size(); ++i )
    {
        if ( m_buttons[i]->isChecked() )
        {
            return m_buttons[i]->text();
        }
    }
    return "";
}

void RadioGroup::indexChanged( bool checked )
{
    if ( checked )
    {
        emit( currentIndexChanged( getCurrentIndex(), m_id ) );
    }
}
