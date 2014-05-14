/*
 * crossingtrackwidget.cpp
 *
 * Created on: 26.01.2013
 * @author Ralph Schurade
 */
#include "crossingtrackwidget.h"

#include "../controls/sliderwithedit.h"
#include "../controls/sliderwitheditint.h"
#include "../controls/selectwithlabel.h"

#include "../../../algos/trackwithcrossings.h"
#include "../../../data/vptr.h"
#include "../../../data/datasets/dataset.h"
#include "../../../data/datasets/datasetfibers.h"
#include "../../../data/datasets/datasetscalar.h"
#include "../../../data/datasets/datasettensor.h"

#include <QPushButton>
#include <QProgressBar>

CrossingTrackWidget::CrossingTrackWidget( Dataset* ds, QList<QVariant> &dsl, QWidget* parent ) :
    m_ds( ds ),
    m_dsl( dsl ),
    m_progress( 0 )
{
    m_tracker = new TrackWithCrossings();
    connect( m_tracker, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
    connect( m_tracker, SIGNAL( finished() ), this, SLOT( slotFinished() ) );

    m_layout = new QVBoxLayout();

    m_maskSelect = new SelectWithLabel( QString( "white matter mask" ), 1 );
    m_maskSelect->insertItem( 0, QString("none") );
    for ( int k = 0; k < dsl.size(); ++k )
    {
        if ( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_TYPE ).toInt() == (int)Fn::DatasetType::NIFTI_SCALAR )
        {
            m_maskSelect->addItem( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_NAME ).toString(), k+1 );
        }
    }
    m_layout->addWidget( m_maskSelect );


    m_tensor2Select = new SelectWithLabel( QString( "tensor dataset 2" ), 1 );
    m_tensor2Select->insertItem( 0, QString("none") );
    for ( int k = 0; k < dsl.size(); ++k )
    {
        if ( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_TYPE ).toInt() == (int)Fn::DatasetType::NIFTI_TENSOR )
        {
            m_tensor2Select->addItem( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_NAME ).toString(), k+1 );
        }
    }
    m_layout->addWidget( m_tensor2Select );

    m_tensor3Select = new SelectWithLabel( QString( "tensor dataset 3" ), 1 );
    m_tensor3Select->insertItem( 0, QString("none") );
    for ( int k = 0; k < dsl.size(); ++k )
    {
        if ( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_TYPE ).toInt() == (int)Fn::DatasetType::NIFTI_TENSOR )
        {
            m_tensor3Select->addItem( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_NAME ).toString(), k+1 );
        }
    }
    m_layout->addWidget( m_tensor3Select );

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_startButton = new QPushButton( tr("Start") );
    connect( m_startButton, SIGNAL( clicked() ), this, SLOT( start() ) );

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

    int numVoxels = ds->properties().get( Fn::Property::D_NX ).toInt() * ds->properties().get( Fn::Property::D_NY ).toInt() * ds->properties().get( Fn::Property::D_NZ ).toInt();

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

CrossingTrackWidget::~CrossingTrackWidget()
{
}

QList<Dataset*> CrossingTrackWidget::getFibs()
{
    QList<Dataset*> l;
    QList<QString>names;
    names.push_back( "FA" );
    DatasetFibers* fibs = new DatasetFibers( QDir( "new fibers" ), m_tracker->getFibs(), names );
    l.push_back( fibs );
    return l;
}

void CrossingTrackWidget::start()
{
    qDebug() << "tensor track widget start";

    qDebug() << m_maskSelect->getCurrentIndex() << m_tensor2Select->getCurrentIndex() << m_tensor3Select->getCurrentIndex();
    qDebug() << VPtr<Dataset>::asPtr( m_dsl[m_maskSelect->getCurrentIndex()-1] )->properties().get( Fn::Property::D_NAME ).toString();
    qDebug() << VPtr<Dataset>::asPtr( m_dsl[m_tensor2Select->getCurrentIndex()-1] )->properties().get( Fn::Property::D_NAME ).toString();
    qDebug() << VPtr<Dataset>::asPtr( m_dsl[m_tensor3Select->getCurrentIndex()-1] )->properties().get( Fn::Property::D_NAME ).toString();
    m_tracker->setDatasets( dynamic_cast<DatasetScalar*>( VPtr<Dataset>::asPtr( m_dsl[m_maskSelect->getCurrentIndex()-1] ) ),
                            dynamic_cast<DatasetTensor*>( m_ds ),
                            dynamic_cast<DatasetTensor*>(VPtr<Dataset>::asPtr( m_dsl[m_tensor2Select->getCurrentIndex()-1] ) ),
                            dynamic_cast<DatasetTensor*>(VPtr<Dataset>::asPtr( m_dsl[m_tensor3Select->getCurrentIndex()-1] ) ) );

    m_startButton->hide();
    m_progressBar->show();
    m_tracker->startTracking();
}

void CrossingTrackWidget::slotProgress()
{
    m_progressBar->setValue( m_progressBar->value() + 100 );
}

void CrossingTrackWidget::slotFinished()
{
    emit( finished() );
}
