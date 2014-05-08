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
#include "../../data/datasets/dataset3d.h"

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

    m_lineWidth = new SliderWithEditInt( "stipple line width");
    buttonlayout->addWidget( m_lineWidth );
    m_lineWidth->setValue( 1 );
    m_lineWidth->setMin( 1 );
    m_lineWidth->setMax( 5 );
    connect( m_lineWidth, SIGNAL( valueChanged( int, int ) ), this, SLOT( lineWidthChanged() ) );

    m_transparencyAdjust = new SliderWithEdit( "transparency adjust");
    buttonlayout->addWidget( m_transparencyAdjust );
    m_transparencyAdjust->setValue( 0.0 );
    m_transparencyAdjust->setMin( -1.0f );
    m_transparencyAdjust->setMax( 1.0f );

    ButtonWithLabel* startButton = new ButtonWithLabel( "start" );
    buttonlayout->addWidget( startButton );
    connect( startButton, SIGNAL( buttonPressed( int ) ), this, SLOT( startAlgo() ) );

    buttonlayout->addStretch();

    m_view = new FiberStipplingGLWidget( "stipple widget", parent, shareWidget );
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
        m_anatomySelect->blockSignals( true );
        m_probTracSelect->blockSignals( true );
        int curScalar = m_anatomySelect->getCurrentIndex();
        int curProb = m_probTracSelect->getCurrentIndex();

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
        if ( dsl1.size() > m_scalarDSL.size() )
        {
            m_anatomySelect->setCurrentIndex( qMax( 0, curScalar ) );
            m_probTracSelect->setCurrentIndex( qMax( 0, curProb ) );
        }
        else
        {
            m_anatomySelect->setCurrentIndex( 0 );
            m_probTracSelect->setCurrentIndex( 0 );
        }
        m_scalarDSL = dsl1;
        updateIsoBounds();
        m_anatomySelect->blockSignals( false );
        m_probTracSelect->blockSignals( false );
    }
    //int filter = (int)Fn::DatasetType::NIFTI_VECTOR | (int)Fn::DatasetType::NIFTI_TENSOR | (int)Fn::DatasetType::NIFTI_DWI;
    int filter = (int)Fn::DatasetType::NIFTI_VECTOR;
    QList<Dataset*>dsl2 = Models::getDatasets( filter );

    if ( dsl2.size() != m_vectorDSL.size() )
    {
        m_vectorSelect->blockSignals( true );
        m_vectorSelect->clear();
        for ( int i = 0; i < dsl2.size(); ++i )
        {
            m_vectorSelect->addItem( dsl2[i]->properties()->get( Fn::Property::D_NAME ).toString(), i+1 );
        }
        m_vectorDSL = dsl2;
        m_vectorSelect->blockSignals( false );
    }
}

void FiberStipplingWidget::updateIsoBounds()
{
    int index = m_anatomySelect->getCurrentIndex();
    if( index >= 0 && m_scalarDSL.size() > 0 )
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
    if( !m_visible )
    {
        return;
    }
    iso1SliderChanged();
    iso2SliderChanged();

    std::vector<float> verts;
    std::vector<float> colors;
    extractVectorsAxial( verts, colors );
    m_view->renderer()->setVectorVerts( verts, colors );

    Models::d()->submit();
}

std::vector<float> FiberStipplingWidget::extractAnatomyAxial()
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

void FiberStipplingWidget::extractVectorsAxial( std::vector<float>& verts, std::vector<float>& colors )
{
    if( m_vectorDSL.size() > 0 && m_scalarDSL.size() > 0 )
    {
        int index = m_vectorSelect->getCurrentIndex();
        Dataset3D* ds = static_cast<Dataset3D*>( m_vectorDSL[ index ] );
        int slicePos =Models::getGlobal( Fn::Property::G_AXIAL ).toInt();
        int sliceDZ =Models::getGlobal( Fn::Property::G_SLICE_DZ ).toInt();
        int nx = ds->properties()->get( Fn::Property::D_NX ).toInt();
        int ny = ds->properties()->get( Fn::Property::D_NY ).toInt();
        float dx = ds->properties()->get( Fn::Property::D_DX ).toFloat();
        float dy = ds->properties()->get( Fn::Property::D_DY ).toFloat();
        float dz = ds->properties()->get( Fn::Property::D_DZ ).toFloat();
        std::vector<QVector3D>* data = ds->getData();

        int index2 = m_probTracSelect->getCurrentIndex();
        DatasetScalar* ds2 = static_cast<DatasetScalar*>( m_scalarDSL[ index2 ] );
        std::vector<float>* probData = ds2->getData();
        //float min = ds2->properties()->get( Fn::Property::D_MIN ).toFloat();
        float max = ds2->properties()->get( Fn::Property::D_MAX ).toFloat();

        float transparencyAdjust = m_transparencyAdjust->getValue();

        for ( int y = 0; y < ny; ++y )
        {
            for ( int x = 0; x < nx; ++x )
            {
                int id = ds->getIdFromPos( x * dx, y * dy, slicePos * sliceDZ );
                QVector3D value = data->at( id );
                value.normalize();

                int id2 = ds2->getIdFromPos( x * dx, y * dy, slicePos * sliceDZ );
                float prob = probData->at( id2 );
                int countStips = qMax( 0, (int)( ( prob / max ) * 10 ) );

                for( int i = 0; i < countStips; ++i )
                {
                    float randx = ( (float) rand() / ( RAND_MAX ) ) * dx - dx/2;
                    float randy = ( (float) rand() / ( RAND_MAX ) ) * dy - dy/2;
                    float randz = ( (float) rand() / ( RAND_MAX ) ) * dz - dz/2;

                    verts.push_back( x * dx + randx );
                    verts.push_back( y * dy + randy );
                    verts.push_back( slicePos * dz + randz);
                    verts.push_back( x * dx + value.x() + randx );
                    verts.push_back( y * dy + value.y() + randy );
                    verts.push_back( slicePos * dz + value.z() + randz );

                    colors.push_back( 1.0 );
                    colors.push_back( 0.0 );
                    colors.push_back( 0.0 );
                    colors.push_back( prob / max + transparencyAdjust );
                    colors.push_back( 1.0 );
                    colors.push_back( 0.0 );
                    colors.push_back( 0.0 );
                    colors.push_back( prob / max + transparencyAdjust );
                }
            }
        }
    }
}

std::vector<float> FiberStipplingWidget::extractAnatomySagittal()
{
    std::vector<float>sliceData;
    if( m_scalarDSL.size() > 0 )
    {
        int index = m_anatomySelect->getCurrentIndex();
        DatasetScalar* ds = static_cast<DatasetScalar*>( m_scalarDSL[ index ] );
        int slicePos =Models::getGlobal( Fn::Property::G_SAGITTAL ).toInt();
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

std::vector<float> FiberStipplingWidget::extractAnatomyCoronal()
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
                sliceData = extractAnatomySagittal();
                MarchingSquares ms1( &sliceData, m_iso1->getValue(), ny, nz, dy, dz );
                m_view->renderer()->setBoundingBox( ny * dy, nz * dz );
                m_view->renderer()->setIso1Verts( ms1.run() );
                break;
            }
            case 1: // coronal
            {
                sliceData = extractAnatomyCoronal();
                MarchingSquares ms1( &sliceData, m_iso1->getValue(), nx, nz, dx, dz );
                m_view->renderer()->setBoundingBox( nx * dx, nz * dz );
                m_view->renderer()->setIso1Verts( ms1.run() );
                break;
            }
            case 2: // axial
            {
                sliceData = extractAnatomyAxial();
                MarchingSquares ms1( &sliceData, m_iso1->getValue(), nx, ny, dx, dy );
                m_view->renderer()->setBoundingBox( nx * dx, ny * dy );
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
    std::vector<QVector3D>vectorData;
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
                sliceData = extractAnatomySagittal();
                MarchingSquares ms2( &sliceData, m_iso2->getValue(), ny, nz, dy, dz );
                m_view->renderer()->setIso2Verts( ms2.run() );
                break;
            }
            case 1: // coronal
            {
                sliceData = extractAnatomyCoronal();
                MarchingSquares ms2( &sliceData, m_iso2->getValue(), nx, nz, dx, dz );
                m_view->renderer()->setIso2Verts( ms2.run() );
                break;
            }
            case 2: // axial
            {
                sliceData = extractAnatomyAxial();
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

void FiberStipplingWidget::lineWidthChanged()
{
    m_view->renderer()->setStippleWidth( m_lineWidth->getValue() );
    Models::d()->submit();
}
