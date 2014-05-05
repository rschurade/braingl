/*
 * mathwidget.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: schurade
 */

#include "mathwidget.h"

#include "../../../data/datasets/datasetscalar.h"

#include "../../../data/models.h"
#include "../../../data/vptr.h"

#include "../controls/sliderwithedit.h"
#include "../controls/sliderwitheditint.h"
#include "../controls/selectwithlabel.h"


MathWidget::MathWidget( DatasetScalar* ds, QWidget* parent ) :
    m_dataset( ds )
{
    m_layout = new QVBoxLayout();

    m_modeSelect = new SelectWithLabel( QString( "mode" ), 1 );
    m_modeSelect->insertItem( 0, QString("make binary") );
    m_modeSelect->insertItem( 1, QString("add") );
    m_modeSelect->insertItem( 2, QString("mult") );
    m_modeSelect->insertItem( 3, QString("between thresholds") );

    m_layout->addWidget( m_modeSelect );
    connect( m_modeSelect, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( modeChanged( int ) ) );

    m_sourceSelect = new SelectWithLabel( QString( "source dataset" ), 1 );

    QList<QVariant>dsl =  Models::d()->data( Models::d()->index( 0, (int)Fn::Property::D_DATASET_LIST ), Qt::DisplayRole ).toList();
    for ( int k = 0; k < dsl.size(); ++k )
    {
        if ( VPtr<Dataset>::asPtr( dsl[k] )->properties()->get( Fn::Property::D_TYPE ).toInt() == (int)Fn::DatasetType::NIFTI_SCALAR )
        {
            m_sourceSelect->addItem( VPtr<Dataset>::asPtr( dsl[k] )->properties()->get( Fn::Property::D_NAME ).toString(), dsl[k] );
        }
    }
    m_layout->addWidget( m_sourceSelect );
    m_sourceSelect->hide();

    m_arg = new SliderWithEdit( QString("arg") );
    m_arg->setMin( -1000 );
    m_arg->setMax( 1000 );
    m_arg->setValue( 0 );
    m_layout->addWidget( m_arg );
    m_arg->hide();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_executeButton = new QPushButton( tr("execute") );
    connect( m_executeButton, SIGNAL( clicked() ), this, SLOT( execute() ) );

    hLayout->addStretch();
    hLayout->addWidget( m_executeButton );
    m_layout->addLayout( hLayout );

    m_layout->addStretch();
    setLayout( m_layout );
}

MathWidget::~MathWidget()
{
}

void MathWidget::modeChanged( int mode )
{
    switch ( mode )
    {
        case 0:
            m_sourceSelect->hide();
            m_arg->hide();
            break;
        case 1:
            m_sourceSelect->hide();
            m_arg->show();
            break;
        case 2:
            m_sourceSelect->hide();
            m_arg->show();
            break;
        case 3:
            m_sourceSelect->hide();
            m_arg->hide();
            break;
    }
}

void MathWidget::execute()
{
    std::vector<float> data = *( m_dataset->getData() );

    int mode = m_modeSelect->getCurrentIndex();
    float arg = m_arg->getValue();

    float lowerThreshold = m_dataset->properties( "maingl" )->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat();
    float upperThreshold = m_dataset->properties( "maingl" )->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat();

    switch ( mode )
    {
        case 0:
        {
            for ( unsigned int i = 0; i < data.size(); ++i )
            {
                if ( data[i] > lowerThreshold && data[i] < upperThreshold )
                {
                    data[i] = 1.0;
                }
                else
                {
                    data[i] = 0.0;
                }
            }
            break;
        }
        case 1:
        {
            for ( unsigned int i = 0; i < data.size(); ++i )
            {
                data[i] += arg;
            }
            break;
        }
        case 2:
        {
            for ( unsigned int i = 0; i < data.size(); ++i )
            {
                data[i] *= arg;
            }
            break;
        }
        case 3:
        {
            for ( unsigned int i = 0; i < data.size(); ++i )
            {
                if ( data[i] < lowerThreshold || data[i] > upperThreshold )
                {
                    data[i] = 0.0;
                }
            }
            break;
        }
    }


    DatasetScalar* out = new DatasetScalar( QDir( "new dataset" ), data, m_dataset->getHeader() );
    QModelIndex index = Models::d()->index( Models::d()->rowCount(), (int)Fn::Property::D_NEW_DATASET );
    Models::d()->setData( index, VPtr<Dataset>::asQVariant( out ), Qt::DisplayRole );

    this->hide();
}
