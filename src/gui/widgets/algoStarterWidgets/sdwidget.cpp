/*
 * sdwidget.cpp
 *
 *  Created on: Jul 22, 2013
 *      Author: schurade
 */

#include "sdwidget.h"

#include "../../../algos/sd.h"
#include "../../../data/datasets/dataset.h"
#include "../../../data/datasets/datasetdwi.h"


SDWidget::SDWidget( Dataset* ds, QList<QVariant> &dsl, QWidget* parent )
{
    m_sd = new SD( dynamic_cast<DatasetDWI*>( ds ) );
    connect( m_sd, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
    connect( m_sd, SIGNAL( finished() ), this, SLOT( slotFinished() ) );

    m_layout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_startButton = new QPushButton( tr("Start") );
    connect( m_startButton, SIGNAL( clicked() ), this, SLOT( start() ) );

    int numVoxels = ds->properties()->get( Fn::Property::D_NX ).toInt() * ds->properties()->get( Fn::Property::D_NY ).toInt() * ds->properties()->get( Fn::Property::D_NZ ).toInt();


    m_progressBar = new QProgressBar( this );
    m_progressBar->setValue( 0 );
    m_progressBar->setMaximum( numVoxels );
    m_progressBar->hide();

    m_layout->addWidget( m_progressBar );

    hLayout->addStretch();
    hLayout->addWidget( m_startButton );

    m_layout->addLayout( hLayout );

    m_layout->addStretch();
    setLayout( m_layout );
}

SDWidget::~SDWidget()
{

}

void SDWidget::start()
{
    qDebug() << "sd widget start";
    m_startButton->hide();
    m_progressBar->show();
    m_sd->start();
}

void SDWidget::slotProgress()
{
    m_progressBar->setValue( m_progressBar->value() + 100 );
}

void SDWidget::slotFinished()
{
    emit( finished() );
}

QList<Dataset*> SDWidget::getResult()
{

}
