/*
 * fiberstipplingwidget.cpp
 *
 *  Created on: May 5, 2014
 *      Author: schurade
 */

#include "fiberstipplingwidget.h"
#include "fiberstipplingglwidget.h"
#include "../gl/fiberstipplingrenderer.h"

#include "controls/buttonwithlabel.h"
#include "controls/colorwidgetwithlabel.h"
#include "controls/selectwithlabel.h"
#include "controls/sliderwithedit.h"
#include "controls/sliderwitheditint.h"

#include "../core_3_3_context.h"

#include "../../algos/marchingsquares.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/datasets/dataset.h"
#include "../../data/datasets/datasetscalar.h"

#include <QtGui>
#include <QDebug>
#include <QImage>

FiberStipplingWidget::FiberStipplingWidget( QString name, QWidget *parent, const QGLWidget *shareWidget ) :
    QWidget( parent ),
    m_visible( true )
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
    m_orientSelect->setCurrentIndex( 2 );
    connect( m_orientSelect, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( iso1SliderChanged() ) );
    connect( m_orientSelect, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( iso2SliderChanged() ) );

    m_iso1 = new SliderWithEdit( "outer iso line");
    buttonlayout->addWidget( m_iso1 );
    m_iso1->setValue( 0 );
    connect( m_iso1, SIGNAL( valueChanged( float, int) ), this, SLOT( iso1SliderChanged() ) );

    m_iso1Color = new ColorWidgetWithLabel( "outer line color" );
    m_iso1Color->setValue( QColor( 0, 0, 0 ) );
    buttonlayout->addWidget( m_iso1Color );
    connect( m_iso1Color, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( iso1ColorChanged() ) );

    m_iso2 = new SliderWithEdit( "inner iso line");
    buttonlayout->addWidget( m_iso2 );
    m_iso2->setValue( 0 );
    connect( m_iso2, SIGNAL( valueChanged( float, int) ), this, SLOT( iso2SliderChanged() ) );

    m_iso2Color = new ColorWidgetWithLabel( "outer line color" );
    m_iso2Color->setValue( QColor( 80, 80, 80 ) );
    buttonlayout->addWidget( m_iso2Color );
    connect( m_iso2Color, SIGNAL( colorChanged( QColor, int ) ), this, SLOT( iso2ColorChanged() ) );

//    ButtonWithLabel* startButton = new ButtonWithLabel( "start" );
//    buttonlayout->addWidget( startButton );
//    connect( startButton, SIGNAL( buttonPressed( int ) ), this, SLOT( startAlgo() ) );

    buttonlayout->addStretch();

    m_view = new FiberStipplingGLWidget( "stipple widget" );
    m_view->setMinimumHeight( 500 );
    m_view->setMinimumWidth( 500 );

    layout1->addLayout( buttonlayout, 20 );
    layout1->addWidget( m_view, 80 );

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
        for ( int i = 0; i < dsl1.size(); ++i )
        {
            m_anatomySelect->addItem( dsl1[i]->properties()->get( Fn::Property::D_NAME ).toString(), i+1 );
        }
        for ( int i = 0; i < dsl1.size(); ++i )
        {
            m_probTracSelect->addItem( dsl1[i]->properties()->get( Fn::Property::D_NAME ).toString(), i+1 );
        }
        m_scalarDSL = dsl1;
        updateIsoBounds();
    }
    int filter = (int)Fn::DatasetType::NIFTI_VECTOR | (int)Fn::DatasetType::NIFTI_TENSOR | (int)Fn::DatasetType::NIFTI_DWI;
    QList<Dataset*>dsl2 = Models::getDatasets( filter );

    if ( dsl2.size() != m_vectorDSL.size() )
    {
        m_vectorSelect->clear();
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
    if( m_scalarDSL.size() > 0 )
    {
        float min = m_scalarDSL[ index ]->properties()->get( Fn::Property::D_MIN ).toFloat();
        float max = m_scalarDSL[ index ]->properties()->get( Fn::Property::D_MAX ).toFloat();

        m_iso1->setMin( min );
        m_iso1->setMax( max );
        m_iso2->setMin( min );
        m_iso2->setMax( max );
    }
}

void FiberStipplingWidget::startAlgo()
{
    iso1SliderChanged();
    iso2SliderChanged();
    Models::d()->submit();
}

std::vector<float> FiberStipplingWidget::extractAxial()
{
    std::vector<float>sliceData;
    if( m_scalarDSL.size() > 0 )
    {
        int index = m_anatomySelect->getCurrentIndex();
        DatasetScalar* ds = static_cast<DatasetScalar*>( m_scalarDSL[ index ] );
        int slicePos =Models::getGlobal( Fn::Property::G_AXIAL ).toInt();
        int nx = ds->properties()->get( Fn::Property::D_NX ).toInt();
        int ny = ds->properties()->get( Fn::Property::D_NY ).toInt();
        std::vector<float>* data = ds->getData();

        sliceData.resize( nx * ny, 0 );

        for ( int y = 0; y < ny; ++y )
        {
            for ( int x = 0; x < nx; ++x )
            {
                int id = ds->getId( x, y, slicePos );
                float value = data->at( id );
                sliceData[ x + nx * y ] = value;
            }
        }
    }
    return sliceData;
}

std::vector<float> FiberStipplingWidget::extractSagittal()
{
    std::vector<float>sliceData;
    if( m_scalarDSL.size() > 0 )
    {
        int index = m_anatomySelect->getCurrentIndex();
        DatasetScalar* ds = static_cast<DatasetScalar*>( m_scalarDSL[ index ] );
        int slicePos =Models::getGlobal( Fn::Property::G_CORONAL ).toInt();
        int ny = ds->properties()->get( Fn::Property::D_NY ).toInt();
        int nz = ds->properties()->get( Fn::Property::D_NZ ).toInt();
        std::vector<float>* data = ds->getData();

        sliceData.resize( ny * nz, 0 );

        for ( int z = 0; z < nz; ++z )
        {
            for ( int y = 0; y < ny; ++y )
            {
                int id = ds->getId( slicePos, y, z );
                float value = data->at( id );
                sliceData[ y + ny * z ] = value;
            }
        }
    }
    return sliceData;
}

std::vector<float> FiberStipplingWidget::extractCoronal()
{
    std::vector<float>sliceData;
    if( m_scalarDSL.size() > 0 )
    {
        int index = m_anatomySelect->getCurrentIndex();
        DatasetScalar* ds = static_cast<DatasetScalar*>( m_scalarDSL[ index ] );
        int slicePos =Models::getGlobal( Fn::Property::G_CORONAL ).toInt();
        int nx = ds->properties()->get( Fn::Property::D_NX ).toInt();
        int nz = ds->properties()->get( Fn::Property::D_NZ ).toInt();
        std::vector<float>* data = ds->getData();

        sliceData.resize( nx * nz, 0 );

        for ( int z = 0; z < nz; ++z )
        {
            for ( int x = 0; x < nx; ++x )
            {
                int id = ds->getId( x, slicePos, z );
                float value = data->at( id );
                sliceData[ x + nx * z ] = value;
            }
        }
    }
    return sliceData;
}

void FiberStipplingWidget::iso1SliderChanged()
{
    std::vector<float>sliceData;
    if( m_scalarDSL.size() > 0 )
    {
        int index = m_anatomySelect->getCurrentIndex();
        DatasetScalar* ds = static_cast<DatasetScalar*>( m_scalarDSL[ index ] );
        int nx = ds->properties()->get( Fn::Property::D_NX ).toInt();
        int ny = ds->properties()->get( Fn::Property::D_NY ).toInt();
        int nz = ds->properties()->get( Fn::Property::D_NZ ).toInt();
        float dx = ds->properties()->get( Fn::Property::D_DX ).toFloat();
        float dy = ds->properties()->get( Fn::Property::D_DY ).toFloat();
        float dz = ds->properties()->get( Fn::Property::D_DZ ).toFloat();

        switch ( m_orientSelect->getCurrentIndex() )
        {
            case 0: // sagittal
            {
                sliceData = extractSagittal();
                MarchingSquares ms1( &sliceData, m_iso1->getValue(), ny, nz, dy, dz );
                m_view->renderer()->setIso1Verts( ms1.run() );
                break;
            }
            case 1: // coronal
            {
                sliceData = extractCoronal();
                MarchingSquares ms1( &sliceData, m_iso1->getValue(), nx, nz, dx, dz );
                m_view->renderer()->setIso1Verts( ms1.run() );
                break;
            }
            case 2: // axial
            {
                sliceData = extractAxial();
                MarchingSquares ms1( &sliceData, m_iso1->getValue(), nx, ny, dx, dy );
                m_view->renderer()->setIso1Verts( ms1.run() );
                break;
            }
        }

        Models::d()->submit();
    }
}

void FiberStipplingWidget::iso2SliderChanged()
{
    std::vector<float>sliceData;
    if( m_scalarDSL.size() > 0 )
    {
        int index = m_anatomySelect->getCurrentIndex();
        DatasetScalar* ds = static_cast<DatasetScalar*>( m_scalarDSL[ index ] );
        int nx = ds->properties()->get( Fn::Property::D_NX ).toInt();
        int ny = ds->properties()->get( Fn::Property::D_NY ).toInt();
        int nz = ds->properties()->get( Fn::Property::D_NZ ).toInt();
        float dx = ds->properties()->get( Fn::Property::D_DX ).toFloat();
        float dy = ds->properties()->get( Fn::Property::D_DY ).toFloat();
        float dz = ds->properties()->get( Fn::Property::D_DY ).toFloat();

        switch ( m_orientSelect->getCurrentIndex() )
        {
            case 0: // sagittal
            {
                sliceData = extractSagittal();
                MarchingSquares ms2( &sliceData, m_iso2->getValue(), ny, nz, dy, dz );
                m_view->renderer()->setIso2Verts( ms2.run() );
                break;
            }
            case 1: // coronal
            {
                sliceData = extractCoronal();
                MarchingSquares ms2( &sliceData, m_iso2->getValue(), nx, nz, dx, dz );
                m_view->renderer()->setIso2Verts( ms2.run() );
                break;
            }
            case 2: // axial
            {
                sliceData = extractAxial();
                MarchingSquares ms2( &sliceData, m_iso2->getValue(), nx, ny, dx, dy );
                m_view->renderer()->setIso2Verts( ms2.run() );
                break;
            }
        }

        Models::d()->submit();
    }
}

void FiberStipplingWidget::iso1ColorChanged()
{
    m_view->renderer()->setIso1Color( m_iso1Color->getValue() );
    Models::d()->submit();
}

void FiberStipplingWidget::iso2ColorChanged()
{
    m_view->renderer()->setIso2Color( m_iso2Color->getValue() );
    Models::d()->submit();
}
