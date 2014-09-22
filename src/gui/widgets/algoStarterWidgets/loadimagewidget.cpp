/*
 * loadimagewidget.cpp
 *
 *  Created on: 26.04.2014
 *      Author: Ralph
 */

#include "loadimagewidget.h"

#include "../controls/sliderwithedit.h"
#include "../controls/sliderwitheditint.h"
#include "../controls/selectwithlabel.h"
#include "../controls/checkbox.h"

#include <QtWidgets>

LoadImageWidget::LoadImageWidget( QWidget* parent ) :
    QDialog( parent )
{
    m_layout = new QVBoxLayout();

    m_orientSelect = new SelectWithLabel( QString( "orientation" ), 1 );
    m_orientSelect->insertItem( 0, QString("sagittal") );
    m_orientSelect->insertItem( 0, QString("coronal") );
    m_orientSelect->insertItem( 0, QString("axial") );
    m_orientSelect->setCurrentIndex( 0 );

    m_layout->addWidget( m_orientSelect );

    m_thickness = new SliderWithEditInt( QString("thickness of image") );
    m_thickness->setMin( 1 );
    m_thickness->setMax( 200 );
    m_thickness->setValue( 1 );
    m_layout->addWidget( m_thickness );

    m_thickness2 = new SliderWithEditInt( QString("thickness of dataset") );
    m_thickness2->setMin( 3 );
    m_thickness2->setMax( 200 );
    m_thickness2->setValue( 3 );
    m_layout->addWidget( m_thickness2 );

    m_offset = new SliderWithEditInt( QString("offset") );
    m_offset->setMin( 0 );
    m_offset->setMax( 200 );
    m_offset->setValue( 0 );
    m_layout->addWidget( m_offset );

//    m_paddingLeftRight = new SliderWithEditInt( QString("padding left/right") );
//    m_paddingLeftRight->setMin( 0 );
//    m_paddingLeftRight->setMax( 200 );
//    m_paddingLeftRight->setValue( 0 );
//    m_layout->addWidget( m_paddingLeftRight );
//
//    m_paddingTopBottom = new SliderWithEditInt( QString("padding top/bottom") );
//    m_paddingTopBottom->setMin( 0 );
//    m_paddingLeftRight->setMax( 200 );
//    m_paddingTopBottom->setValue( 0 );
//    m_layout->addWidget( m_paddingTopBottom );

    m_whiteTransparent = new CheckBox( "white is transparent" );
    m_whiteTransparent->setChecked( false );
    m_layout->addWidget( m_whiteTransparent );

    m_layout->addStretch();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_okButton = new QPushButton( tr("Ok") );
    hLayout->addStretch();
    hLayout->addWidget( m_okButton );
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( ok() ) );

    m_layout->addLayout( hLayout );

    setLayout( m_layout );
}

LoadImageWidget::~LoadImageWidget()
{
}

void LoadImageWidget::ok()
{
    accept();
}

int LoadImageWidget::getOrient()
{
    return m_orientSelect->getCurrentIndex();
}
