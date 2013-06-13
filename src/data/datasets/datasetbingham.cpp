/*
 * datasetbingham.cpp
 *
 * Created on: Nov 23, 2012
 * @author Ralph Schurade
 */
#include "datasetbingham.h"
#include "../models.h"

#include "../../gui/gl/binghamrenderer.h"

DatasetBingham::DatasetBingham( QDir filename, QVector<QVector<float> > data, nifti_image* header ) :
    DatasetNifti( filename, Fn::DatasetType::NIFTI_BINGHAM, header ),
    m_data( data ),
    m_renderer( 0 )
{
    m_properties["maingl"]->set( Fn::Property::SCALING, 1.0f, 0.1f, 2.0f, "general" );
    m_properties["maingl"]->set( Fn::Property::OFFSET, 0, -1, 1, "general" );
    m_properties["maingl"]->set( Fn::Property::ORDER, 4 );
    m_properties["maingl"]->set( Fn::Property::LOD, 2, 0, 4, "general" );
    m_properties["maingl"]->set( Fn::Property::RENDER_FIRST, true, "general" );
    m_properties["maingl"]->set( Fn::Property::RENDER_SECOND, false, "general" );
    m_properties["maingl"]->set( Fn::Property::RENDER_THIRD, false, "general" );
    m_properties["maingl"]->set( Fn::Property::RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"]->set( Fn::Property::RENDER_CORONAL, false, "general" );
    m_properties["maingl"]->set( Fn::Property::RENDER_AXIAL, true, "general" );

    examineDataset();
}

DatasetBingham::~DatasetBingham()
{
    m_properties["maingl"]->set( Fn::Property::ACTIVE, false );
    delete m_renderer;
}

QVector<QVector<float> >* DatasetBingham::getData()
{
    return &m_data;
}

void DatasetBingham::examineDataset()
{
    int type = m_properties["maingl"]->get( Fn::Property::DATATYPE ).toInt();
    int nx = m_properties["maingl"]->get( Fn::Property::NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::NZ ).toInt();
    int dim = m_data.at( 0 ).size();
    m_properties["maingl"]->set( Fn::Property::DIM, dim );
    int size = nx * ny * nz * dim;

    if ( type == DT_FLOAT )
    {
        m_properties["maingl"]->set( Fn::Property::SIZE, static_cast<int>( size * sizeof(float) ) );
        m_properties["maingl"]->set( Fn::Property::MIN, -1.0f );
        m_properties["maingl"]->set( Fn::Property::MAX, 1.0f );
    }
    m_properties["maingl"]->set( Fn::Property::LOWER_THRESHOLD, m_properties["maingl"]->get( Fn::Property::MIN ).toFloat() );
    m_properties["maingl"]->set( Fn::Property::UPPER_THRESHOLD, m_properties["maingl"]->get( Fn::Property::MAX ).toFloat() );
}

void DatasetBingham::createTexture()
{
}

void DatasetBingham::flipX()
{
}

void DatasetBingham::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::ACTIVE ).toBool() )
    {
        return;
    }
    if ( m_renderer == 0 )
    {
        qDebug() << "ds bingham init renderer";
        m_renderer = new BinghamRenderer( &m_data, properties( target )->get( Fn::Property::NX ).toInt(),
                                                  properties( target )->get( Fn::Property::NY ).toInt(),
                                                  properties( target )->get( Fn::Property::NZ ).toInt(),
                                                  properties( target )->get( Fn::Property::DX ).toFloat(),
                                                  properties( target )->get( Fn::Property::DY ).toFloat(),
                                                  properties( target )->get( Fn::Property::DZ ).toFloat() );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
        qDebug() << "ds bingham init renderer done";
    }

    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
}

QString DatasetBingham::getValueAsString( int x, int y, int z )
{
    float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    x *= dx;
    y *= dy;
    z *= dz;

    QVector<float> data = m_data[ getIdFromPos( x, y, z ) ];
    return QString::number( data[0] ) + ", " + QString::number( data[1] ) + ", " + QString::number( data[2] ) + ", " + QString::number( data[3] ) +
     ", " + QString::number( data[4] ) + ", " + QString::number( data[5] ) + ", " + QString::number( data[6] ) + ", " + QString::number( data[7] ) +
     ", " + QString::number( data[8] );
}
