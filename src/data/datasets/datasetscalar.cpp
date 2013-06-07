/*
 * datasetscalar.cpp
 *
 * Created on: May 4, 2012
 * @author Ralph Schurade
 */
#include "datasetscalar.h"
#include "../models.h"

#include "../../gui/gl/colormaprenderer.h"

#include <QDebug>

DatasetScalar::DatasetScalar( QDir filename, QVector<float> data, nifti_image* header ) :
        DatasetNifti( filename, Fn::DatasetType::NIFTI_SCALAR, header ), m_data( data ),
        m_colormapRenderer( 0 )
{
    m_properties["maingl"]->set( Fn::Property::INTERPOLATION, false, "general" );
    m_properties["maingl"]->set( Fn::Property::ALPHA, 1.0f, 0.0, 1.0, "general" );
    m_properties["maingl"]->set( Fn::Property::DIM, 1 );
    m_properties["maingl"]->set( Fn::Property::HAS_TEXTURE, true );

    m_properties["maingl2"]->set( Fn::Property::INTERPOLATION, false, "general" );
    m_properties["maingl2"]->set( Fn::Property::ALPHA, 1.0f, 0.0, 1.0, "general" );
    m_properties["maingl2"]->set( Fn::Property::COLORMAP, 0, "general" );
    m_properties["maingl2"]->set( Fn::Property::DIM, 1 );
    m_properties["maingl2"]->set( Fn::Property::HAS_TEXTURE, true );

    examineDataset();

    m_properties["maingl"]->set( Fn::Property::COLORMAP, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl"]->set( Fn::Property::COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl"]->set( Fn::Property::COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );
    m_properties["maingl"]->set( Fn::Property::COLORMAP_TEXT_COLOR, QColor( 1, 1, 1 ), "colormap" );
    m_properties["maingl"]->set( Fn::Property::IS_ATLAS, false, "colormap" );

    m_properties["maingl2"]->set( Fn::Property::RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::COLORMAP_TEXT_COLOR, QColor( 1, 1, 1 ), "colormap" );
}

DatasetScalar::~DatasetScalar()
{
    m_data.clear();
    glDeleteTextures( 1, &m_textureGLuint );
}

QVector<float>* DatasetScalar::getData()
{
    return &m_data;
}


void DatasetScalar::examineDataset()
{
    int nx = m_properties["maingl"]->get( Fn::Property::NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::NZ ).toInt();

    float min = std::numeric_limits<float>::max();
    float max = 0;

    int size = nx * ny * nz;
    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, m_data[i] );
        max = qMax( max, m_data[i] );
    }
    max += 1.0f;
    m_properties["maingl"]->set( Fn::Property::SIZE, static_cast<int>( size * sizeof(float) ) );
    m_properties["maingl"]->set( Fn::Property::MIN, min );
    m_properties["maingl"]->set( Fn::Property::MAX, max );
    m_properties["maingl"]->set( Fn::Property::SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl"]->set( Fn::Property::SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl"]->set( Fn::Property::LOWER_THRESHOLD, min + (max-min)/1000., min, max, "general" );
    m_properties["maingl"]->set( Fn::Property::UPPER_THRESHOLD, max, min, max, "general" );

    connect( m_properties["maingl"]->getProperty( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::LOWER_THRESHOLD ), SLOT( setMax( float ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::UPPER_THRESHOLD ), SLOT( setMin( float ) ) );

    connect( m_properties["maingl"]->getProperty( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::SELECTED_MAX ), SLOT( setMin( float ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl"]->getProperty( Fn::Property::SELECTED_MIN ), SLOT( setMax( float ) ) );

    m_properties["maingl2"]->set( Fn::Property::SIZE, static_cast<int>( size * sizeof(float) ) );
    m_properties["maingl2"]->set( Fn::Property::MIN, min );
    m_properties["maingl2"]->set( Fn::Property::MAX, max );
    m_properties["maingl2"]->set( Fn::Property::SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl2"]->set( Fn::Property::SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl2"]->set( Fn::Property::LOWER_THRESHOLD, min + (max-min)/1000., min, max, "general" );
    m_properties["maingl2"]->set( Fn::Property::UPPER_THRESHOLD, max, min, max, "general" );

    connect( m_properties["maingl2"]->getProperty( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl2"]->getProperty( Fn::Property::LOWER_THRESHOLD ), SLOT( setMax( float ) ) );
    connect( m_properties["maingl2"]->getProperty( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl2"]->getProperty( Fn::Property::UPPER_THRESHOLD ), SLOT( setMin( float ) ) );

    connect( m_properties["maingl2"]->getProperty( Fn::Property::SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl2"]->getProperty( Fn::Property::SELECTED_MAX ), SLOT( setMin( float ) ) );
    connect( m_properties["maingl2"]->getProperty( Fn::Property::SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
              m_properties["maingl2"]->getProperty( Fn::Property::SELECTED_MIN ), SLOT( setMax( float ) ) );


    if ( m_qform.Determinant() < 0.0 && m_qform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties["maingl"]->get( Fn::Property::NAME ).toString() << ": RADIOLOGICAL orientation in q-form detected. Flipping voxels on X-Axis";
        flipX();
    }
    else
    {
        if ( m_sform.Determinant() < 0.0 && m_sform( 1, 1 ) < 0 )
        {
            qDebug() << m_properties["maingl"]->get( Fn::Property::NAME ).toString() << ": RADIOLOGICAL orientation in s-form detected. Flipping voxels on X-Axis";
            flipX();
        }
    }

    m_properties["maingl"]->set( Fn::Property::PAINTMODE, { "off", "paint" }, 0, "paint" );
    m_properties["maingl"]->set( Fn::Property::PAINTSIZE, 1, 1, 5, "paint" );
    m_properties["maingl"]->set( Fn::Property::PAINTVALUE, min, min, max - 1.0, "paint" );
}

void DatasetScalar::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

    int nx = m_properties["maingl"]->get( Fn::Property::NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::NZ ).toInt();

    float min = m_properties["maingl"]->get( Fn::Property::MIN ).toFloat();
    float max = m_properties["maingl"]->get( Fn::Property::MAX ).toFloat();

    unsigned char* tmpData = new unsigned char[nx * ny * nz * 4];
    for ( int i = 0; i < nx * ny * nz; ++i )
    {
        //unsigned int tmp = (double)i / (double)(nx * ny * nz) * 256 * 256 * 256 * 256;
        unsigned int tmp = ( ( m_data[i] - min ) / ( max - min ) ) * 256 * 256 * 256 * 256;
        tmpData[4 * i + 3 ] = (tmp / (256 * 256 * 256)) % 256;
        tmpData[4 * i + 2 ] = (tmp / (256 * 256)) % 256;
        tmpData[4 * i + 1 ] = (tmp / (256)) % 256;
        tmpData[4 * i + 0 ] = tmp % 256 ;
    }

    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmpData );
    delete[] tmpData;
}

void DatasetScalar::flipX()
{
    int nx = m_properties["maingl"]->get( Fn::Property::NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::NZ ).toInt();

    QVector<float> newData;

    for ( int z = 0; z < nz; ++z )
    {
        for ( int y = 0; y < ny; ++y )
        {
            for ( int x = nx - 1; x >= 0; --x )
            {
                newData.push_back( m_data[x + y * nx + z * nx * ny] );
            }
        }
    }

    m_header->qoffset_x = 0.0;

    m_header->qto_xyz.m[0][0] = qMax( m_header->qto_xyz.m[0][0], m_header->qto_xyz.m[0][0] * -1.0f );
    m_header->sto_xyz.m[0][0] = qMax( m_header->sto_xyz.m[0][0], m_header->sto_xyz.m[0][0] * -1.0f );
    m_header->qto_xyz.m[0][3] = 0.0;
    m_header->sto_xyz.m[0][3] = 0.0;

    m_data.clear();
    m_data = newData;
}

void DatasetScalar::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::ACTIVE ).toBool() )
    {
        return;
    }
    if ( properties( target )->get( Fn::Property::RENDER_COLORMAP ).toBool() )
    {
        if ( !m_colormapRenderer )
        {
            m_colormapRenderer = new ColormapRenderer();
            m_colormapRenderer->init();
        }
        m_colormapRenderer->setColormap( properties( target )->get( Fn::Property::COLORMAP ).toInt() );
        m_colormapRenderer->setX( properties( target )->get( Fn::Property::COLORMAP_X ).toFloat() );
        m_colormapRenderer->setY( properties( target )->get( Fn::Property::COLORMAP_Y ).toFloat() );
        m_colormapRenderer->setDX( properties( target )->get( Fn::Property::COLORMAP_DX ).toFloat() );
        m_colormapRenderer->setDY( properties( target )->get( Fn::Property::COLORMAP_DY ).toFloat() );
        m_colormapRenderer->setTextSize( properties( target )->get( Fn::Property::COLORMAP_TEXT_SIZE ).toFloat() );
        m_colormapRenderer->setTextColor( properties( target )->get( Fn::Property::COLORMAP_TEXT_COLOR ).value<QColor>() );

        m_colormapRenderer->setMin( properties( target )->get( Fn::Property::MIN).toFloat() );
        m_colormapRenderer->setMax( properties( target )->get( Fn::Property::MAX).toFloat() );
        m_colormapRenderer->setSelectedMin( properties( target )->get( Fn::Property::SELECTED_MIN).toFloat() );
        m_colormapRenderer->setSelectedMax( properties( target )->get( Fn::Property::SELECTED_MAX).toFloat() );
        m_colormapRenderer->setLowerThreshold( properties( target )->get( Fn::Property::LOWER_THRESHOLD).toFloat() );
        m_colormapRenderer->setUpperThreshold( properties( target )->get( Fn::Property::UPPER_THRESHOLD).toFloat() );

        m_colormapRenderer->draw( width, height, renderMode );
    }
}

QString DatasetScalar::getValueAsString( int x, int y, int z )
{
    float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    x *= dx;
    y *= dy;
    z *= dz;

    float data = m_data[ getIdFromPos( x, y, z ) ];
    return QString::number( data );
}

void DatasetScalar::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers )
{
    int paintMode = m_properties["maingl"]->get( Fn::Property::PAINTMODE ).toInt();
    if ( pickId == 0 ||  paintMode == 0 || !( modifiers & Qt::ControlModifier ) )
    {
        return;
    }

    float dx = m_properties["maingl"]->get( Fn::Property::DX ).toFloat();
    float dy = m_properties["maingl"]->get( Fn::Property::DY ).toFloat();
    float dz = m_properties["maingl"]->get( Fn::Property::DZ ).toFloat();

    float paintValue = m_properties["maingl"]->get( Fn::Property::PAINTVALUE ).toFloat();
    m_data[ getIdFromPos( pos.x(), pos.y(), pos.z() ) ] = paintValue;

    int brushSize = m_properties["maingl"]->get( Fn::Property::PAINTSIZE ).toInt();

    for ( int i = 0; i < brushSize; ++i )
    {
       for ( int j = 0; j < brushSize; ++j )
       {
           for ( int k = 0; k < brushSize; ++k )
           {
               m_data[ getIdFromPos( pos.x() - i * dx, pos.y() - j * dy, pos.z() - k * dz ) ] = paintValue;
               m_data[ getIdFromPos( pos.x() - i * dx, pos.y() - j * dy, pos.z() + k * dz ) ] = paintValue;
               m_data[ getIdFromPos( pos.x() - i * dx, pos.y() + j * dy, pos.z() - k * dz ) ] = paintValue;
               m_data[ getIdFromPos( pos.x() - i * dx, pos.y() + j * dy, pos.z() + k * dz ) ] = paintValue;
               m_data[ getIdFromPos( pos.x() + i * dx, pos.y() - j * dy, pos.z() - k * dz ) ] = paintValue;
               m_data[ getIdFromPos( pos.x() + i * dx, pos.y() - j * dy, pos.z() + k * dz ) ] = paintValue;
               m_data[ getIdFromPos( pos.x() + i * dx, pos.y() + j * dy, pos.z() - k * dz ) ] = paintValue;
               m_data[ getIdFromPos( pos.x() + i * dx, pos.y() + j * dy, pos.z() + k * dz ) ] = paintValue;
           }
       }
    }

    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;
    Models::d()->submit();
}
