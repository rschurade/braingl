/*
 * datasetfmri.cpp
 *
 *  Created on: Aug 9, 2013
 *      Author: schurade
 */

#include "datasetfmri.h"
#include "../models.h"

#include <QTimer>

DatasetFMRI::DatasetFMRI( QDir filename, QVector<float> data, nifti_image* header ) :
    DatasetNifti( filename, Fn::DatasetType::NIFTI_FMRI, header ),
    m_data( data )
{
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, 0, 0, (int)Fn::ColormapEnum::NONE - 1, "color" );
    m_properties["maingl"]->create( Fn::Property::D_INTERPOLATION, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0, "general" );

    examineDataset();

    m_properties["maingl"]->create( Fn::Property::D_HAS_TEXTURE, true );

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"]->getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );

    m_properties["maingl"]->create( Fn::Property::D_AUTOPLAY, false, "autoplay" );
    m_properties["maingl"]->create( Fn::Property::D_AUTOPLAY_INTERVAL, 25, 10, 1000, "autoplay" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_AUTOPLAY ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( autoplay() ) );
}

DatasetFMRI::~DatasetFMRI()
{
    // TODO Auto-generated destructor stub
}

QVector<float>* DatasetFMRI::getData()
{
    return &m_data;
}


void DatasetFMRI::examineDataset()
{
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();
    int dim = m_properties["maingl"]->get( Fn::Property::D_DIM ).toInt();

    float min = std::numeric_limits<float>::max();
    float max = 0;

    int size = nx * ny * nz * dim;
    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, m_data[i] );
        max = qMax( max, m_data[i] );
    }

    m_properties["maingl"]->create( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(float) ) );
    m_properties["maingl"]->create( Fn::Property::D_MIN, min );
    m_properties["maingl"]->create( Fn::Property::D_MAX, max );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/100., min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "general" );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( QVariant ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( QVariant ) ) );


    if ( m_qform.Determinant() < 0.0 && m_qform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties["maingl"]->get( Fn::Property::D_NAME ).toString() << ": RADIOLOGICAL orientation in q-form detected. Flipping voxels on X-Axis";
        flipX();
    }
    else
    {
        if ( m_sform.Determinant() < 0.0 && m_sform( 1, 1 ) < 0 )
        {
            qDebug() << m_properties["maingl"]->get( Fn::Property::D_NAME ).toString() << ": RADIOLOGICAL orientation in s-form detected. Flipping voxels on X-Axis";
            flipX();
        }
    }

    m_properties["maingl"]->create( Fn::Property::D_PAINTMODE, { "off", "paint" }, 0, "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINTSIZE, 1, 1, 10, "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINTVALUE, max - 1.0, min, max - 1.0, "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINT_LINK_CURSOR, false, "paint" );

    m_properties["maingl"]->create( Fn::Property::D_SELECTED_TEXTURE, 0, 0, dim - 1, "general" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_TEXTURE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( selectTexture() ) );
    connect ( m_properties["maingl"], SIGNAL( signalSetProp( int ) ), this, SLOT( slotPropSet( int ) ) );
}

void DatasetFMRI::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    float min = m_properties["maingl"]->get( Fn::Property::D_MIN ).toFloat();
    float max = m_properties["maingl"]->get( Fn::Property::D_MAX ).toFloat();
    max = max + ( (max-min)/100. );

    int dim = m_properties["maingl"]->get( Fn::Property::D_DIM ).toInt();
    int frame = m_properties["maingl"]->get( Fn::Property::D_SELECTED_TEXTURE ).toInt();
    if ( frame < 0 )
    {
        frame = 0;
    }

    if ( frame > dim - 1 )
    {
        frame = dim - 1;
    }
    int offset = nx * ny * nz * frame;

    unsigned char* tmpData = new unsigned char[nx * ny * nz * 4];
    for ( int i = 0; i < nx * ny * nz; ++i )
    {
        unsigned int tmp = ( ( m_data[i + offset] - min ) / ( max - min ) ) * 256 * 256 * 256 * 256;
        tmpData[4 * i + 3 ] = (tmp / (256 * 256 * 256)) % 256;
        tmpData[4 * i + 2 ] = (tmp / (256 * 256)) % 256;
        tmpData[4 * i + 1 ] = (tmp / (256)) % 256;
        tmpData[4 * i + 0 ] = tmp % 256 ;
    }

    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmpData );
    delete[] tmpData;
}

void DatasetFMRI::flipX()
{
//    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
//    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
//    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();
//
//    QVector<float> newData;
//
//    for ( int z = 0; z < nz; ++z )
//    {
//        for ( int y = 0; y < ny; ++y )
//        {
//            for ( int x = nx - 1; x >= 0; --x )
//            {
//                newData.push_back( m_data[x + y * nx + z * nx * ny] );
//            }
//        }
//    }
//
//    m_header->qoffset_x = 0.0;
//
//    m_header->qto_xyz.m[0][0] = qMax( m_header->qto_xyz.m[0][0], m_header->qto_xyz.m[0][0] * -1.0f );
//    m_header->sto_xyz.m[0][0] = qMax( m_header->sto_xyz.m[0][0], m_header->sto_xyz.m[0][0] * -1.0f );
//    m_header->qto_xyz.m[0][3] = 0.0;
//    m_header->sto_xyz.m[0][3] = 0.0;
//
//    m_data.clear();
//    m_data = newData;
}

void DatasetFMRI::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
}

QString DatasetFMRI::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}

bool DatasetFMRI::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    return false;
}

void DatasetFMRI::selectTexture()
{
    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;
}

void DatasetFMRI::autoplay()
{
    if ( m_properties["maingl"]->get( Fn::Property::D_AUTOPLAY ).toBool() )
    {
        int dim = m_properties["maingl"]->get( Fn::Property::D_DIM ).toInt();
        int currentframe = m_properties["maingl"]->get( Fn::Property::D_SELECTED_TEXTURE ).toInt();
        ++currentframe;
        if ( currentframe >= dim )
        {
            currentframe = 0;
        }
        m_properties["maingl"]->set( Fn::Property::D_SELECTED_TEXTURE, currentframe );
        selectTexture();
        Models::d()->submit();

        int interval = m_properties["maingl"]->get( Fn::Property::D_AUTOPLAY_INTERVAL ).toInt();
        QTimer::singleShot( interval, this, SLOT( autoplay() ) );
    }
}

void DatasetFMRI::slotPropSet( int id )
{
    if ( id == (int)Fn::Property::D_SELECTED_TEXTURE )
    {
        selectTexture();
    }
}
