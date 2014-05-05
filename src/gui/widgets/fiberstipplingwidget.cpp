/*
 * fiberstipplingwidget.cpp
 *
 *  Created on: May 5, 2014
 *      Author: schurade
 */

#include "fiberstipplingwidget.h"

#include "controls/selectwithlabel.h"
#include "controls/sliderwithedit.h"

#include "../core_3_3_context.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/datasets/dataset.h"

#include <QtGui>
#include <QDebug>

FiberStipplingWidget::FiberStipplingWidget( QString name, QWidget *parent, const QGLWidget *shareWidget ) :
    QWidget( parent )
{
    QHBoxLayout* layout1 = new QHBoxLayout();

    QVBoxLayout* buttonlayout = new QVBoxLayout();

    m_anatomySelect = new SelectWithLabel( QString( "anatomy" ), 1 );
    m_vectorSelect = new SelectWithLabel( QString( "vector dataset" ), 1 );
    m_probTracSelect = new SelectWithLabel( QString( "probabilistic dataset" ), 1 );
    connect( m_anatomySelect, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( updateIsoBounds() ) );

    buttonlayout->addWidget( m_anatomySelect );
    buttonlayout->addWidget( m_vectorSelect );
    buttonlayout->addWidget( m_probTracSelect );

    m_orientSelect = new SelectWithLabel( QString( "orientation" ), 1 );
    m_orientSelect->insertItem( 0, QString("sagittal") );
    m_orientSelect->insertItem( 1, QString("coronal") );
    m_orientSelect->insertItem( 2, QString("axial") );
    buttonlayout->addWidget( m_orientSelect );

    m_iso1 = new SliderWithEdit( "outer iso line");
    buttonlayout->addWidget( m_iso1 );

    m_iso2 = new SliderWithEdit( "inner iso line");
    buttonlayout->addWidget( m_iso2 );


    buttonlayout->addStretch();

    m_edit = new QTextEdit( tr("edit" ) );
    m_edit->setMinimumHeight( 500 );
    m_edit->setMinimumWidth( 500 );

    layout1->addLayout( buttonlayout, 20 );
    layout1->addWidget( m_edit, 80 );

    this->setLayout( layout1 );
    this->setMinimumSize( 600, 600 );
}

FiberStipplingWidget::~FiberStipplingWidget()
{
}
void FiberStipplingWidget::update()
{
    if ( !m_visible )
    {
        return;
    }
}

void FiberStipplingWidget::setWidgetVisible( bool visible )
{
    m_visible = visible;
}

void FiberStipplingWidget::updateSelects()
{
    QList<Dataset*>dsl1 = Models::getDatasets( Fn::DatasetType::NIFTI_SCALAR );

    if( dsl1.size() != m_scalarDSL.size() )
    {
        m_anatomySelect->clear();
        m_probTracSelect->clear();
        m_anatomySelect->insertItem( 0, QString("none") );
        for ( int i = 0; i < dsl1.size(); ++i )
        {
            m_anatomySelect->addItem( dsl1[i]->properties()->get( Fn::Property::D_NAME ).toString(), i+1 );
        }
        m_probTracSelect->insertItem( 0, QString("none") );
        for ( int i = 0; i < dsl1.size(); ++i )
        {
            m_probTracSelect->addItem( dsl1[i]->properties()->get( Fn::Property::D_NAME ).toString(), i+1 );
        }
        m_scalarDSL = dsl1;
    }
    int filter = (int)Fn::DatasetType::NIFTI_VECTOR | (int)Fn::DatasetType::NIFTI_TENSOR | (int)Fn::DatasetType::NIFTI_DWI;
    QList<Dataset*>dsl2 = Models::getDatasets( filter );

    if ( dsl2.size() != m_vectorDSL.size() )
    {
        m_vectorSelect->clear();
        m_vectorSelect->insertItem( 0, QString("none") );
        for ( int i = 0; i < dsl2.size(); ++i )
        {
            m_vectorSelect->addItem( dsl2[i]->properties()->get( Fn::Property::D_NAME ).toString(), i+1 );
        }
        m_vectorDSL = dsl2;
    }
}

void FiberStipplingWidget::updateIsoBounds()
{
    int index = m_anatomySelect->getCurrentIndex();
    if( index > 0 && m_scalarDSL.size() > 0 )
    {
        float min = m_scalarDSL[ index - 1 ]->properties()->get( Fn::Property::D_MIN ).toFloat();
        float max = m_scalarDSL[ index - 1 ]->properties()->get( Fn::Property::D_MAX ).toFloat();

        m_iso1->setMin( min );
        m_iso1->setMax( max );
        m_iso2->setMin( min );
        m_iso2->setMax( max );
    }
}
