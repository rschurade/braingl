/*
 * selectwithlabel.cpp
 *
 * Created on: 12.07.2012
 * @author Ralph Schurade
 */
#include "selectwithlabel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>

SelectWithLabel::SelectWithLabel( QString label, int id, QWidget* parent ) :
    QFrame( parent )
{
    m_id = id;

    m_comboBox = new QComboBox();
    connect( m_comboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( indexChanged( int ) ) );

    QVBoxLayout* vLayout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_label = new QLabel( label );
    hLayout->addWidget( m_label );
    hLayout->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget( m_comboBox );

    vLayout->addLayout( hLayout );
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

SelectWithLabel::~SelectWithLabel()
{
    delete this->layout();
}

void SelectWithLabel::insertItem( int index, QString label )
{
    m_comboBox->insertItem( index, label );
}

void SelectWithLabel::addItem ( const QString &text, const QVariant &userData )
{
    m_comboBox->addItem( text, userData );
}

void SelectWithLabel::setCurrentIndex( int index )
{
    m_comboBox->setCurrentIndex( index );
}

int SelectWithLabel::getCurrentIndex()
{
    return m_comboBox->currentIndex();
}

void SelectWithLabel::indexChanged( int index )
{
    emit( currentIndexChanged( index, m_id ) );
}

QString SelectWithLabel::getSelectedItemString()
{
    return m_comboBox->currentText();
}

QVariant SelectWithLabel::getSelectedItemData()
{
    return m_comboBox->itemData( m_comboBox->currentIndex() );
}
