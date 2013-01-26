/*
 * tensortrackwidget.cpp
 *
 *  Created on: 26.01.2013
 *      Author: Ralph
 */
#include <QtGui/QPushButton>
#include <QtGui/QProgressBar>

#include "sliderwithedit.h"
#include "sliderwitheditint.h"
#include "selectwithlabel.h"

#include "../../algos/track.h"
#include "../../data/datasets/dataset.h"
#include "../../data/datasets/datasetFibers.h"
#include "../../data/datasets/datasetTensor.h"

#include "tensortrackwidget.h"

TensorTrackWidget::TensorTrackWidget( Dataset* ds, QList<Dataset*> &dsl, QWidget* parent ) :
    m_progress( 0 )
{
    m_tracker = new Track( dynamic_cast<DatasetTensor*>( ds ) );
    connect( m_tracker, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
    connect( m_tracker, SIGNAL( finished() ), this, SLOT( slotFinished() ) );

    m_layout = new QVBoxLayout();
/*
    SelectWithLabel* sel = new SelectWithLabel( QString( "optional: mask (not yet implemented)" ), 1 );
    sel->insertItem( 0, QString("none") );
    for ( int k = 0; k < dsl.size(); ++k )
    {
        if ( dsl[k]->properties()->get( FNPROP_TYPE ).toInt() == FNDT_NIFTI_SCALAR )
        {
            sel->insertItem( k+1, dsl[k]->properties()->get( FNPROP_NAME ).toString() );
        }
    }
    m_layout->addWidget( sel );

    SelectWithLabel* sel2 = new SelectWithLabel( QString( "optional: roi (not yet implemented)" ), 1 );
    sel2->insertItem( 0, QString("none") );
    for ( int k = 0; k < dsl.size(); ++k )
    {
        if ( dsl[k]->properties()->get( FNPROP_TYPE ).toInt() == FNDT_NIFTI_SCALAR )
        {
            sel2->insertItem( k+1, dsl[k]->properties()->get( FNPROP_NAME ).toString() );
        }
    }
    m_layout->addWidget( sel2 );
*/
    QHBoxLayout* hLayout = new QHBoxLayout();
    m_startButton = new QPushButton( tr("Start") );
    connect( m_startButton, SIGNAL( clicked() ), this, SLOT( start() ) );

    SliderWithEdit* minFA = new SliderWithEdit( QString("min FA for Start Voxels") );
    minFA->setMin( 0.01f );
    minFA->setMax( 1.0f );
    minFA->setValue( 0.2f );
    m_layout->addWidget( minFA );
    connect( minFA, SIGNAL( valueChanged( float, int) ), m_tracker, SLOT( setMinStartFA( float, int) ) );

    SliderWithEdit* stopFA = new SliderWithEdit( QString("min FA to keep tracking") );
    stopFA->setMin( 0.01f );
    stopFA->setMax( 1.0f );
    stopFA->setValue( 0.2f );
    m_layout->addWidget( stopFA );
    connect( stopFA, SIGNAL( valueChanged( float, int) ), m_tracker, SLOT( setMinFA( float, int) ) );

    SliderWithEditInt* minLength = new SliderWithEditInt( QString("min length to keep fibers") );
    minLength->setMin( 1 );
    minLength->setMax( 200 );
    minLength->setValue( 10 );
    m_layout->addWidget( minLength );
    connect( minLength, SIGNAL( valueChanged( int, int) ), m_tracker, SLOT( setMinLength( int, int) ) );

    SliderWithEdit* setSize = new SliderWithEdit( QString("step size") );
    setSize->setMin( 0.01f );
    setSize->setMax( 2.0f );
    setSize->setValue( 1.0f );
    m_layout->addWidget( setSize );
    connect( setSize, SIGNAL( valueChanged( float, int) ), m_tracker, SLOT( setStepSize( float, int) ) );

    SliderWithEdit* smoothness = new SliderWithEdit( QString("smoothness") );
    smoothness->setMin( 0.0f );
    smoothness->setMax( 1.0f );
    smoothness->setValue( 0.0f );
    m_layout->addWidget( smoothness );
    connect( smoothness, SIGNAL( valueChanged( float, int) ), m_tracker, SLOT( setSmoothness( float, int) ) );


    int numVoxels = ds->properties()->get( FNPROP_NX ).toInt() * ds->properties()->get( FNPROP_NY ).toInt() * ds->properties()->get( FNPROP_NZ ).toInt();


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

TensorTrackWidget::~TensorTrackWidget()
{
}

QList<Dataset*> TensorTrackWidget::getFibs()
{
    QList<Dataset*> l;
    DatasetFibers* fibs = new DatasetFibers( m_tracker->getFibs() );
    l.push_back( fibs );
    return l;
}

void TensorTrackWidget::start()
{
    qDebug() << "tensor track widget start";
    m_startButton->hide();
    m_progressBar->show();
    m_tracker->startTracking();
}

void TensorTrackWidget::slotProgress()
{
    m_progressBar->setValue( m_progressBar->value() + 100 );
}

void TensorTrackWidget::slotFinished()
{
    emit( finished() );
}
