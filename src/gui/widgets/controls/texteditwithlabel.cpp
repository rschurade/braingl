/*
 * texteditwithlabel.cpp
 *
 * Created on: 15.11.2013
 * @author Ralph Schurade
 */
#include "texteditwithlabel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>

TextEditWithLabel::TextEditWithLabel( QString label, int id, QWidget* parent ) :
    QFrame( parent )
{
    m_id = id;

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( label, this );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    m_textEdit = new QTextEdit( this );
    hLayout2->addWidget( m_textEdit );
    //hLayout2->addStretch();

    vLayout->addLayout( hLayout );
    vLayout->addLayout( hLayout2 );
    vLayout->setContentsMargins( 1,1,1,1 );

    setLayout( vLayout );

    connect( m_textEdit, SIGNAL( editingFinished() ), this, SLOT( slotTextChanged() ) );

    setFrameStyle( QFrame::Panel | QFrame::Raised );

    setStyleSheet( "QLabel { font:  bold 12px; margin-bottom: -1px }"
                     "QLineEdit { font: 12px; max-height: 14px; margin-top: -1px }"
                   );
}


TextEditWithLabel::~TextEditWithLabel()
{
    delete this->layout();
}

void TextEditWithLabel::slotTextChanged()
{
    emit( valueChanged( m_textEdit->toPlainText(), m_id ) );
}

void TextEditWithLabel::setText( QString text )
{
    m_textEdit->setText( text );
}

QString TextEditWithLabel::text()
{
    return m_textEdit->toPlainText();
}
