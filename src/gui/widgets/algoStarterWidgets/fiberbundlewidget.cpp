/*
 * fiberbundlewidget.cpp
 *
 * Created on: 04.11.2013
 * @author Ralph Schurade
 */
#include "fiberbundlewidget.h"

#include "../controls/sliderwithedit.h"
#include "../controls/sliderwitheditint.h"
#include "../controls/selectwithlabel.h"

#include "../../../algos/bundle.h"
#include "../../../data/datasets/dataset.h"
#include "../../../data/datasets/datasetfibers.h"

#include "../../gui/gl/glfunctions.h"

#include <QPushButton>
#include <QProgressBar>

FiberBundleWidget::FiberBundleWidget( Dataset* ds, QWidget* parent ) :
    m_progress( 0 )
{
    m_bundler = new Bundle( dynamic_cast<DatasetFibers*>( ds ) );
    connect( m_bundler, SIGNAL( progress() ), this, SLOT( slotProgress() ), Qt::QueuedConnection );
    connect( m_bundler, SIGNAL( finished() ), this, SLOT( slotFinished() ) );

    m_layout = new QVBoxLayout();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_startButton = new QPushButton( tr("Start") );
    connect( m_startButton, SIGNAL( clicked() ), this, SLOT( start() ) );

    m_iterations = new SliderWithEditInt( QString("iterations") );
    m_iterations->setMin( 1 );
    m_iterations->setMax( 200 );
    m_iterations->setValue( 10 );
    m_layout->addWidget( m_iterations );
    connect( m_iterations, SIGNAL( valueChanged( int, int) ), m_bundler, SLOT( setIterations( int, int) ) );

    SliderWithEdit* radius = new SliderWithEdit( QString("radius to apply forces from") );
    radius->setMin( 1.0f );
    radius->setMax( 20.0f );
    radius->setValue( 10.0f );
    m_layout->addWidget( radius );
    connect( radius, SIGNAL( valueChanged( float, int) ), m_bundler, SLOT( setRadius( float, int) ) );

    SliderWithEdit* smoothRange = new SliderWithEdit( QString("force dampening at fiber start/end") );
    smoothRange->setMin( 0.0f );
    smoothRange->setMax( 40.0f );
    smoothRange->setValue( 10.0f );
    m_layout->addWidget( smoothRange );
    connect( smoothRange, SIGNAL( valueChanged( float, int) ), m_bundler, SLOT( setSmoothRange( float, int) ) );


//
//    SliderWithEdit* setSize = new SliderWithEdit( QString("step size") );
//    setSize->setMin( 0.01f );
//    setSize->setMax( 2.0f );
//    setSize->setValue( 1.0f );
//    m_layout->addWidget( setSize );
//    connect( setSize, SIGNAL( valueChanged( float, int) ), m_bundler, SLOT( setStepSize( float, int) ) );
//
//    SliderWithEdit* smoothness = new SliderWithEdit( QString("smoothness") );
//    smoothness->setMin( 0.0f );
//    smoothness->setMax( 1.0f );
//    smoothness->setValue( 0.0f );
//    m_layout->addWidget( smoothness );
//    connect( smoothness, SIGNAL( valueChanged( float, int) ), m_bundler, SLOT( setSmoothness( float, int) ) );

    m_progressBar = new QProgressBar( this );
    m_progressBar->setValue( 0 );
    m_progressBar->setMaximum( 10 * GLFunctions::idealThreadCount );
    m_progressBar->hide();

    m_layout->addWidget( m_progressBar );

    hLayout->addStretch();
    hLayout->addWidget( m_startButton );

    m_layout->addLayout( hLayout );

    m_layout->addStretch();
    setLayout( m_layout );
}

FiberBundleWidget::~FiberBundleWidget()
{
}

QList<Dataset*> FiberBundleWidget::getFibs()
{
    QList<Dataset*> l;

    QVector<QString>names;
    names.push_back( "FA" );
    std::vector<float>mins;
    mins.push_back( 0.0 );
    std::vector<float>maxes;
    maxes.push_back( 1.0 );
    DatasetFibers* fibs = new DatasetFibers( QDir( "new fibers" ), m_bundler->getFibs() );
    l.push_back( fibs );
    return l;
}

void FiberBundleWidget::start()
{
    qDebug() << "fiber bundle widget start";
    m_startButton->hide();
    m_progressBar->setMaximum( m_iterations->getValue() * GLFunctions::idealThreadCount );
    m_progressBar->show();
    m_bundler->start();
}

void FiberBundleWidget::slotProgress()
{
    m_progressBar->setValue( m_progressBar->value() + 1 );
}

void FiberBundleWidget::slotFinished()
{
    emit( finished() );
}
