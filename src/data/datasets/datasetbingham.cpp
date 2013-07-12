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
    m_properties["maingl"]->create( Fn::Property::D_SCALING, 1.0f, 0.1f, 2.0f, "general" );
    m_properties["maingl"]->create( Fn::Property::D_OFFSET, 0, -1, 1, "general" );
    m_properties["maingl"]->create( Fn::Property::D_ORDER, 4 );
    m_properties["maingl"]->create( Fn::Property::D_LOD, 2, 0, 4, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_FIRST, true, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_SECOND, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_THIRD, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_CORONAL, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_AXIAL, true, "general" );

    examineDataset();
}

DatasetBingham::~DatasetBingham()
{
    m_properties["maingl"]->set( Fn::Property::D_ACTIVE, false );
    delete m_renderer;
}

QVector<QVector<float> >* DatasetBingham::getData()
{
    return &m_data;
}

void DatasetBingham::examineDataset()
{
    int type = m_properties["maingl"]->get( Fn::Property::D_DATATYPE ).toInt();
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();
    int dim = m_data.at( 0 ).size();
    m_properties["maingl"]->create( Fn::Property::D_DIM, dim );
    int size = nx * ny * nz * dim;

    if ( type == DT_FLOAT )
    {
        m_properties["maingl"]->create( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(float) ) );
        m_properties["maingl"]->create( Fn::Property::D_MIN, -1.0f );
        m_properties["maingl"]->create( Fn::Property::D_MAX, 1.0f );
    }
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, m_properties["maingl"]->get( Fn::Property::D_MIN ).toFloat() );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, m_properties["maingl"]->get( Fn::Property::D_MAX ).toFloat() );
}

void DatasetBingham::createTexture()
{
}

void DatasetBingham::flipX()
{
}

void DatasetBingham::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    if ( m_renderer == 0 )
    {
        qDebug() << "ds bingham init renderer";
        m_renderer = new BinghamRenderer( &m_data, properties( target )->get( Fn::Property::D_NX ).toInt(),
                                                  properties( target )->get( Fn::Property::D_NY ).toInt(),
                                                  properties( target )->get( Fn::Property::D_NZ ).toInt(),
                                                  properties( target )->get( Fn::Property::D_DX ).toFloat(),
                                                  properties( target )->get( Fn::Property::D_DY ).toFloat(),
                                                  properties( target )->get( Fn::Property::D_DZ ).toFloat() );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
        qDebug() << "ds bingham init renderer done";
    }

    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
}

QString DatasetBingham::getValueAsString( int x, int y, int z )
{
    QVector<float> data = m_data[ getId( x, y, z ) ];
    return QString::number( data[0] ) + ", " + QString::number( data[1] ) + ", " + QString::number( data[2] ) + ", " + QString::number( data[3] ) +
     ", " + QString::number( data[4] ) + ", " + QString::number( data[5] ) + ", " + QString::number( data[6] ) + ", " + QString::number( data[7] ) +
     ", " + QString::number( data[8] );
}
