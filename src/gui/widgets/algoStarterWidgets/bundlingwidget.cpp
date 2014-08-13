/*
 * bundlingwidget.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: boettgerj
 */

#include "bundlingwidget.h"

#include "../controls/sliderwithedit.h"
#include "../controls/sliderwitheditint.h"
#include "../controls/selectwithlabel.h"

#include <QPushButton>
#include <QProgressBar>

BundlingWidget::BundlingWidget( Connections* cons, QWidget* parent ) :
        m_progress( 0 )
{

    m_cons = cons;
    connect( cons, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
    connect( cons, SIGNAL( finished() ), this, SLOT( slotFinished() ) );

    m_layout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_startButton = new QPushButton( tr( "Bundle" ) );
    connect( m_startButton, SIGNAL( clicked() ), this, SLOT( start() ) );

    SliderWithEdit* cthr = new SliderWithEdit( QString( "compatibility threshold" ) );
    cthr->setMin( 0.01f );
    cthr->setMax( 1.0f );
    cthr->setValue( 0.8f );
    m_layout->addWidget( cthr );
    connect( cthr, SIGNAL( valueChanged( float, int) ), m_cons, SLOT( setCthr( float, int) ) );

    SliderWithEdit* bell = new SliderWithEdit( QString( "mean shift kernel radius" ) );
    bell->setMin( 0.01f );
    bell->setMax( 100.0f );
    bell->setValue( 5.0f );
    m_layout->addWidget( bell );
    connect( bell, SIGNAL( valueChanged( float, int) ), m_cons, SLOT( setBell( float, int) ) );

    SliderWithEditInt* smooth = new SliderWithEditInt( QString( "smoothing steps" ) );
    smooth->setMin( 1 );
    smooth->setMax( 10 );
    smooth->setValue( 3 );
    m_layout->addWidget( smooth );
    connect( smooth, SIGNAL( valueChanged( int, int) ), m_cons, SLOT( setSmooth( int, int) ) );

    int iterations = 10;//TODO: calculate number of iterations

    m_progressBar = new QProgressBar( this );
    m_progressBar->setValue( 0 );
    m_progressBar->setMaximum( iterations );
    m_progressBar->hide();

    m_layout->addWidget( m_progressBar );

    hLayout->addStretch();
    hLayout->addWidget( m_startButton );

    m_layout->addLayout( hLayout );

    m_layout->addStretch();
    setLayout( m_layout );
}

BundlingWidget::~BundlingWidget()
{
}

QList<Dataset*> BundlingWidget::getBundles()
{
    return m_cons->createDatasetFibers();
}

void BundlingWidget::start()
{
    qDebug() << "bundling widget start";
    m_startButton->hide();
    m_progressBar->show();
    m_cons->startBundling();
}

void BundlingWidget::slotProgress()
{
    //TODO:
    m_progressBar->setValue( m_progressBar->value() + 1 );
}

void BundlingWidget::slotFinished()
{
    emit( finished() );
}
