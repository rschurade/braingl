/*
 * newdatasetwidget.cpp
 *
 * Created on: 26.01.2013
 * @author Ralph Schurade
 */
#include "newdatasetwidget.h"

#include "../controls/sliderwithedit.h"
#include "../controls/sliderwitheditint.h"
#include "../controls/selectwithlabel.h"

#include "../roiwidget.h"

#include "../../../data/models.h"
#include "../../../data/vptr.h"
#include "../../../data/roiarea.h"

#include "../../../data/datasets/dataset.h"
#include "../../../data/datasets/datasetscalar.h"

#include "../../../io/writer.h"

#include <QPushButton>
#include <QProgressBar>

NewDatasetWidget::NewDatasetWidget( ROIWidget* roiWidget, QWidget* parent ) :
    m_roiWidget( roiWidget )
{
    m_layout = new QVBoxLayout();

    m_modeSelect = new SelectWithLabel( QString( "mode" ), 1 );
    m_modeSelect->insertItem( 0, QString("new") );
    m_modeSelect->insertItem( 1, QString("copy") );
    m_modeSelect->insertItem( 2, QString("copy with roi selection") );

    m_layout->addWidget( m_modeSelect );
    connect( m_modeSelect, SIGNAL( currentIndexChanged( int, int ) ), this, SLOT( modeChanged( int ) ) );

    m_sourceSelect = new SelectWithLabel( QString( "source dataset" ), 1 );

    QList<QVariant>dsl =  Models::d()->data( Models::d()->index( 0, (int)Fn::Property::D_DATASET_LIST ), Qt::DisplayRole ).toList();
    for ( int k = 0; k < dsl.size(); ++k )
    {
        if ( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_TYPE ).toInt() == (int)Fn::DatasetType::NIFTI_SCALAR )
        {
            m_sourceSelect->addItem( VPtr<Dataset>::asPtr( dsl[k] )->properties().get( Fn::Property::D_NAME ).toString(), dsl[k] );
        }
    }
    m_layout->addWidget( m_sourceSelect );
    m_sourceSelect->hide();

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_createDatasetButton = new QPushButton( tr("Create dataset") );
    connect( m_createDatasetButton, SIGNAL( clicked() ), this, SLOT( createDataset() ) );

    m_createROIButton = new QPushButton( tr("Create roi") );
    connect( m_createROIButton, SIGNAL( clicked() ), this, SLOT( createROI() ) );
    m_createROIButton->hide();

    m_nX = new SliderWithEditInt( QString("nx") );
    m_nX->setMin( 1 );
    m_nX->setMax( 500 );
    m_nX->setValue( 160 );
    m_layout->addWidget( m_nX );

    m_nY = new SliderWithEditInt( QString("ny") );
    m_nY->setMin( 1 );
    m_nY->setMax( 500 );
    m_nY->setValue( 200 );
    m_layout->addWidget( m_nY );

    m_nZ = new SliderWithEditInt( QString("nz") );
    m_nZ->setMin( 1 );
    m_nZ->setMax( 500 );
    m_nZ->setValue( 160 );
    m_layout->addWidget( m_nZ );

    m_dX = new SliderWithEdit( QString("dx") );
    m_dX->setMin( 0.1f );
    m_dX->setMax( 5.0f );
    m_dX->setValue( 1.0f );
    m_layout->addWidget( m_dX );

    m_dY = new SliderWithEdit( QString("dy") );
    m_dY->setMin( 0.1f );
    m_dY->setMax( 5.0f );
    m_dY->setValue( 1.0f );
    m_layout->addWidget( m_dY );

    m_dZ = new SliderWithEdit( QString("dz") );
    m_dZ->setMin( 0.1f );
    m_dZ->setMax( 5.0f );
    m_dZ->setValue( 1.0f );
    m_layout->addWidget( m_dZ );

    hLayout->addStretch();
    hLayout->addWidget( m_createROIButton );
    hLayout->addWidget( m_createDatasetButton );

    m_layout->addLayout( hLayout );

    m_layout->addStretch();
    setLayout( m_layout );



}

NewDatasetWidget::~NewDatasetWidget()
{
}

void NewDatasetWidget::createDataset()
{
    switch( m_modeSelect->getCurrentIndex() )
    {
        case 0:
        {
            int nx = m_nX->getValue();
            int ny = m_nY->getValue();
            int nz = m_nZ->getValue();
            float dx = m_dX->getValue();
            float dy = m_dY->getValue();
            float dz = m_dZ->getValue();
            std::vector<float> data( nx * ny * nz, 0 );
//            for( int z = 0; z < nz; ++z )
//            {
//                for( int y = 0; y < ny; ++y )
//                {
//                    for( int x = 0; x < nx; ++x )
//                    {
//                        float val = 255 * ( ( ( x + ( y % 2 ) ) + ( z % 2 ) )  % 2 ) ;
//                        data[ x + y*nx + z*nx*ny ] = val;
//                    }
//                }
//            }

            data[0] = 255;
            int dims[8] = { 3, nx, ny, nz, 1, 1, 1 };
            nifti_image* header = nifti_make_new_nim( dims, NIFTI_TYPE_FLOAT32, 1 );
            header->dx = dx;
            header->dy = dy;
            header->dz = dz;
            DatasetScalar* out = new DatasetScalar( QDir( "new dataset" ), data, header );

            QModelIndex index = Models::d()->index( Models::d()->rowCount(), (int)Fn::Property::D_NEW_DATASET );
            Models::d()->setData( index, VPtr<Dataset>::asQVariant( out ), Qt::DisplayRole );
            break;
        }
        case 1:
        {
            DatasetScalar* ds = static_cast<DatasetScalar*>( VPtr<Dataset>::asPtr( m_sourceSelect->getSelectedItemData() ) );

            std::vector<float>* data = ds->getData();
            std::vector<float> out( data->size() );

            float min = ds->properties( "maingl" ).get( Fn::Property::D_SELECTED_MIN ).toFloat();
            float max = ds->properties( "maingl" ).get( Fn::Property::D_SELECTED_MAX ).toFloat();
            float totalMin = ds->properties( "maingl" ).get( Fn::Property::D_MIN ).toFloat();
            float totalMax = ds->properties( "maingl" ).get( Fn::Property::D_MAX ).toFloat();
            float lowerThreshold = ds->properties( "maingl" ).get( Fn::Property::D_LOWER_THRESHOLD ).toFloat();
            float upperThreshold = ds->properties( "maingl" ).get( Fn::Property::D_UPPER_THRESHOLD ).toFloat();

            for ( unsigned int i = 0; i < data->size(); ++i )
            {
                float value = data->at( i );
                if ( value < lowerThreshold || value > upperThreshold )
                {
                    value = 0.0f;
                }
                else
                {
                    value = ( value - min ) / ( max - min );
                    value *= totalMax;
                }
                value = qMax( totalMin, qMin( totalMax, value ) );

                out[i] = value;
            }

            Writer writer( ds, QFileInfo() );
            DatasetScalar* dsOut = new DatasetScalar( QDir( "new dataset" ), out, writer.createHeader( 1 ) );

            QModelIndex index = Models::d()->index( Models::d()->rowCount(), (int)Fn::Property::D_NEW_DATASET );
            Models::d()->setData( index, VPtr<Dataset>::asQVariant( dsOut ), Qt::DisplayRole );

            break;
        }
        case 2:
        {
            DatasetScalar* ds = static_cast<DatasetScalar*>( VPtr<Dataset>::asPtr( m_sourceSelect->getSelectedItemData() ) );

            std::vector<float>* data = ds->getData();
            std::vector<float> out( data->size() );

            copyWithRois( ds, out );

            Writer writer( ds, QFileInfo() );
            DatasetScalar* dsOut = new DatasetScalar( QDir( "new dataset" ), out, writer.createHeader( 1 ) );

            QModelIndex index = Models::d()->index( Models::d()->rowCount(), (int)Fn::Property::D_NEW_DATASET );
            Models::d()->setData( index, VPtr<Dataset>::asQVariant( dsOut ), Qt::DisplayRole );

            break;
        }
    }
    this->hide();
}

void NewDatasetWidget::createROI()
{
    DatasetScalar* ds = static_cast<DatasetScalar*>( VPtr<Dataset>::asPtr( m_sourceSelect->getSelectedItemData() ) );

    std::vector<float>* data = ds->getData();
    std::vector<float> out( data->size() );

    switch( m_modeSelect->getCurrentIndex() )
    {
        case 1:
            {
                float min = ds->properties( "maingl" ).get( Fn::Property::D_SELECTED_MIN ).toFloat();
                float max = ds->properties( "maingl" ).get( Fn::Property::D_SELECTED_MAX ).toFloat();
                float totalMin = ds->properties( "maingl" ).get( Fn::Property::D_MIN ).toFloat();
                float totalMax = ds->properties( "maingl" ).get( Fn::Property::D_MAX ).toFloat();
                float lowerThreshold = ds->properties( "maingl" ).get( Fn::Property::D_LOWER_THRESHOLD ).toFloat();
                float upperThreshold = ds->properties( "maingl" ).get( Fn::Property::D_UPPER_THRESHOLD ).toFloat();

                for ( unsigned int i = 0; i < data->size(); ++i )
                {
                    float value = data->at( i );
                    if ( value < lowerThreshold || value > upperThreshold )
                    {
                        value = 0.0f;
                    }
                    else
                    {
                        value = ( value - min ) / ( max - min );
                        value *= totalMax;
                    }
                    value = qMax( totalMin, qMin( totalMax, value ) );

                    out[i] = value;
                }
            }
            break;
        case 2 :
            copyWithRois( ds, out );
            break;
    }

    ROIArea* roiOut = new ROIArea( out, ds->properties() );

    m_roiWidget->addROIArea( roiOut );

    this->hide();
}

void NewDatasetWidget::modeChanged( int mode )
{
    switch ( mode )
    {
        case 0:
            m_sourceSelect->hide();
            m_nX->show();
            m_nY->show();
            m_nZ->show();
            m_dX->show();
            m_dY->show();
            m_dZ->show();
            m_createROIButton->hide();
            break;
        case 1:
        case 2:
            m_sourceSelect->show();
            m_nX->hide();
            m_nY->hide();
            m_nZ->hide();
            m_dX->hide();
            m_dY->hide();
            m_dZ->hide();
            m_createROIButton->show();

    }
}

QModelIndex NewDatasetWidget::createIndex( int branch, int pos, int column )
{
    int row;
    QModelIndex parent;
    if ( pos == 0 )
    {
        row = branch;
    }
    else
    {
        row = pos - 1;
        parent = Models::r()->index( branch, 0 );
    }
    return Models::r()->index( row, column, parent );
}

void NewDatasetWidget::copyWithRois( DatasetScalar* source, std::vector<float> &target )
{
    int numBranches = Models::r()->rowCount( QModelIndex() );

    for ( int i = 0; i < numBranches; ++i )
    {
        copy( i, 0, source, target );

        int leafCount = Models::r()->rowCount( createIndex( i, 0, 0 ) );

        for ( int k = 0; k < leafCount; ++k )
        {
            copy( i, k + 1, source, target );
        }
    }
}

void NewDatasetWidget::copy( int branch, int pos, DatasetScalar* source, std::vector<float> &target )
{
    if ( Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_SHAPE ), Qt::DisplayRole ).toInt() > 4 )
    {
        return;
    }

    std::vector<float>* s = source->getData();

    int ds_nx = source->properties().get( Fn::Property::D_NX ).toInt();
    int ds_ny = source->properties().get( Fn::Property::D_NY ).toInt();
    int ds_nz = source->properties().get( Fn::Property::D_NZ ).toInt();
    float ds_dx = source->properties().get( Fn::Property::D_DX ).toFloat();
    float ds_dy = source->properties().get( Fn::Property::D_DY ).toFloat();
    float ds_dz = source->properties().get( Fn::Property::D_DZ ).toFloat();

    if ( Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_ACTIVE ), Qt::DisplayRole ).toBool() )
    {

        float x = Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_X ), Qt::DisplayRole ).toFloat();
        float y = Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_Y ), Qt::DisplayRole ).toFloat();
        float z = Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_Z ), Qt::DisplayRole ).toFloat();
        float dx = Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_DX ), Qt::DisplayRole ).toFloat() / 2;
        float dy = Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_DY ), Qt::DisplayRole ).toFloat() / 2;
        float dz = Models::r()->data( createIndex( branch, pos, (int)Fn::Property::D_DZ ), Qt::DisplayRole ).toFloat() / 2;
        float xMin = x - dx;
        float xMax = x + dx;
        float yMin = y - dy;
        float yMax = y + dy;
        float zMin = z - dz;
        float zMax = z + dz;

        for ( int iz = 0; iz < ds_nz; ++iz )
        {
            for ( int iy = 0; iy < ds_ny; ++iy )
            {
                for ( int ix = 0; ix < ds_nx; ++ix )
                {
                    float vx = (float)ix * ds_dx;
                    float vy = (float)iy * ds_dy;
                    float vz = (float)iz * ds_dz;

                    if ( vx >= xMin && vx <= xMax && vy >= yMin && vy <= yMax && vz >= zMin && vz <= zMax )
                    {
                        target[ ix + ds_nx * iy + iz * ds_nx * ds_ny ] = s->at( ix + ds_nx * iy + iz * ds_nx * ds_ny );
                    }
                }
            }
        }
    }
}
