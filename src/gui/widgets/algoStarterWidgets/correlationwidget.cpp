/*
 * CorrelationWidget.cpp
 *
 * Created on: 11.09.2013
 * @author Ralph Schurade
 */
#include "correlationwidget.h"

#include "../controls/sliderwithedit.h"
#include "../controls/sliderwitheditint.h"
#include "../controls/selectwithlabel.h"

#include "../../../algos/correlation.h"

#include "../../../data/datasets/dataset.h"
#include "../../../data/datasets/datasetcorrelation.h"
#include "../../../data/datasets/datasetglyphset.h"
#include "../../../data/datasets/datasetmeshtimeseries.h"

#include "../../../data/mesh/trianglemesh2.h"

#include <QPushButton>
#include <QProgressBar>

CorrelationWidget::CorrelationWidget( Dataset* ds, QWidget* parent ) :
    m_progress( 0 )
{
    m_dataset = dynamic_cast<DatasetMeshTimeSeries*>( ds );
    m_correlation = new Correlation( m_dataset );
    connect( m_correlation, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
    connect( m_correlation, SIGNAL( finished() ), this, SLOT( slotFinished() ) );

    m_layout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_startButton = new QPushButton( tr("Start") );
    connect( m_startButton, SIGNAL( clicked() ), this, SLOT( start() ) );

    int numVertexes = ds->properties().get( Fn::Property::D_MESH_NUM_VERTEX ).toInt();

    m_progressBar = new QProgressBar( this );
    m_progressBar->setValue( 0 );
    m_progressBar->setMaximum( numVertexes );
    m_progressBar->hide();

    m_layout->addWidget( m_progressBar );

    hLayout->addStretch();
    hLayout->addWidget( m_startButton );

    m_layout->addLayout( hLayout );

    m_layout->addStretch();
    setLayout( m_layout );
}

CorrelationWidget::~CorrelationWidget()
{
}

QList<Dataset*> CorrelationWidget::getResult()
{
//    QList<Dataset*> l;
//    DatasetGlyphset* ds = new DatasetGlyphset( QDir( "new glyphset" ), 0.5, 1.0 );
//    ds->addMesh( m_dataset->getMesh(), "mesh" );
//    ds->addCorrelation( m_correlation->getResult() );
//    ds->initROI();
//    ds->setProperties();
//    l.push_back( ds );
//    return l;

    QList<Dataset*> l;
    DatasetCorrelation* ds = new DatasetCorrelation( QDir( "new correlation" ), 0.5, 1.0 );
    ds->addMesh( m_dataset->getMesh(), "mesh" );
    ds->setCorrelationMatrix( m_correlation->getResult() );
    //ds->initROI();
    ds->setProperties();
    l.push_back( ds );
    return l;

}

void CorrelationWidget::start()
{
    qDebug() << "tensor track widget start";
    m_startButton->hide();
    m_progressBar->show();

    m_correlation->start();
}

void CorrelationWidget::slotProgress()
{
    m_progressBar->setValue( m_progressBar->value() + 100 );
}

void CorrelationWidget::slotFinished()
{
    emit( finished() );
}
