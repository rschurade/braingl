/*
 * datasetscalar.cpp
 *
 * Created on: May 4, 2012
 * @author Ralph Schurade
 */
#include "datasetscalar.h"
#include "../models.h"

#include "../../gui/gl/colormapfunctions.h"
#include "../../gui/gl/colormaprenderer.h"

#include <QDebug>

DatasetScalar::DatasetScalar( QDir filename, std::vector<float> data, nifti_image* header ) :
        DatasetNifti( filename, Fn::DatasetType::NIFTI_SCALAR, header ), m_data( data ),
        m_colormapRenderer( 0 )
{
    m_properties["maingl"]->createBool( Fn::Property::D_INTERPOLATION, false, "general" );
    m_properties["maingl"]->createFloat( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0, "general" );
    m_properties["maingl"]->createInt( Fn::Property::D_DIM, 1 );
    m_properties["maingl"]->createBool( Fn::Property::D_HAS_TEXTURE, true );

    examineDataset();

    m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP, 0, "general" );
    m_properties["maingl"]->createBool( Fn::Property::D_RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );
    m_properties["maingl"]->createColor( Fn::Property::D_COLORMAP_TEXT_COLOR, QColor( 1, 1, 1 ), "colormap" );
    //m_properties["maingl"]->create( Fn::Property::D_IS_ATLAS, false, "colormap" );

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"]->getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );
    m_properties["maingl2"]->getProperty( Fn::Property::D_LOCK_PROPS )->setPropertyTab( "general" );

//    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
//              m_properties["maingl2"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
//    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
//              m_properties["maingl2"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( QVariant ) ) );
    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( QVariant ) ) );
}

DatasetScalar::~DatasetScalar()
{
    m_properties["maingl"]->set( Fn::Property::D_ACTIVE, false );
    delete m_colormapRenderer;
    m_data.clear();
    glDeleteTextures( 1, &m_textureGLuint );
}

std::vector<float>* DatasetScalar::getData()
{
    return &m_data;
}


void DatasetScalar::examineDataset()
{
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    float min = std::numeric_limits<float>::max();
    float max = 0;

    int size = nx * ny * nz;
    for ( int i = 0; i < size; ++i )
    {
        min = qMin( min, m_data[i] );
        max = qMax( max, m_data[i] );
    }

    m_properties["maingl"]->createInt( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(float) ) );
    m_properties["maingl"]->createFloat( Fn::Property::D_MIN, min );
    m_properties["maingl"]->createFloat( Fn::Property::D_MAX, max );
    m_properties["maingl"]->createFloat( Fn::Property::D_SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl"]->createFloat( Fn::Property::D_SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl"]->createFloat( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/100., min, max, "general" );
    m_properties["maingl"]->createFloat( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "general" );

//    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
//              m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
//    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
//              m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( QVariant ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
              m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( QVariant ) ) );

    m_properties["maingl"]->createList( Fn::Property::D_PAINTMODE, { "off", "paint" }, 0, "paint" );
    m_properties["maingl"]->createInt( Fn::Property::D_PAINTSIZE, 1, 1, 10, "paint" );
    m_properties["maingl"]->createFloat( Fn::Property::D_PAINTVALUE, max - 1.0, min, max - 1.0, "paint" );
    m_properties["maingl"]->createBool( Fn::Property::D_PAINT_LINK_CURSOR, false, "paint" );
}

void DatasetScalar::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, /*GL_CLAMP*/ GL_CLAMP_TO_EDGE );    // XXX CoreProfile
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, /*GL_CLAMP*/ GL_CLAMP_TO_EDGE );    // XXX CoreProfile
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, /*GL_CLAMP*/ GL_CLAMP_TO_EDGE );    // XXX CoreProfile

    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    float min = m_properties["maingl"]->get( Fn::Property::D_MIN ).toFloat();
    float max = m_properties["maingl"]->get( Fn::Property::D_MAX ).toFloat();
    max = max + ( (max-min)/100. );

    unsigned char* tmpData = new unsigned char[nx * ny * nz * 4];
    for ( int i = 0; i < nx * ny * nz; ++i )
    {
        unsigned int tmp = ( ( m_data[i] - min ) / ( max - min ) ) * 256 * 256 * 256 * 256;
        tmpData[4 * i + 3 ] = (tmp / (256 * 256 * 256)) % 256;
        tmpData[4 * i + 2 ] = (tmp / (256 * 256)) % 256;
        tmpData[4 * i + 1 ] = (tmp / (256)) % 256;
        tmpData[4 * i + 0 ] = tmp % 256 ;
    }

    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmpData );
    delete[] tmpData;
}

void DatasetScalar::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    if ( properties( target )->get( Fn::Property::D_LOCK_PROPS ).toBool() )
    {
        target = "maingl";
    }

    if ( properties( target )->get( Fn::Property::D_RENDER_COLORMAP ).toBool() )
    {
        if ( !m_colormapRenderer )
        {
            m_colormapRenderer = new ColormapRenderer();
            m_colormapRenderer->init();
        }
        m_colormapRenderer->setColormap( properties( target )->get( Fn::Property::D_COLORMAP ).toInt() );
        m_colormapRenderer->setX( properties( target )->get( Fn::Property::D_COLORMAP_X ).toFloat() );
        m_colormapRenderer->setY( properties( target )->get( Fn::Property::D_COLORMAP_Y ).toFloat() );
        m_colormapRenderer->setDX( properties( target )->get( Fn::Property::D_COLORMAP_DX ).toFloat() );
        m_colormapRenderer->setDY( properties( target )->get( Fn::Property::D_COLORMAP_DY ).toFloat() );
        m_colormapRenderer->setTextSize( properties( target )->get( Fn::Property::D_COLORMAP_TEXT_SIZE ).toFloat() );
        m_colormapRenderer->setTextColor( properties( target )->get( Fn::Property::D_COLORMAP_TEXT_COLOR ).value<QColor>() );

        m_colormapRenderer->setMin( properties( target )->get( Fn::Property::D_MIN).toFloat() );
        m_colormapRenderer->setMax( properties( target )->get( Fn::Property::D_MAX).toFloat() );
        m_colormapRenderer->setSelectedMin( properties( target )->get( Fn::Property::D_SELECTED_MIN).toFloat() );
        m_colormapRenderer->setSelectedMax( properties( target )->get( Fn::Property::D_SELECTED_MAX).toFloat() );
        m_colormapRenderer->setLowerThreshold( properties( target )->get( Fn::Property::D_LOWER_THRESHOLD).toFloat() );
        m_colormapRenderer->setUpperThreshold( properties( target )->get( Fn::Property::D_UPPER_THRESHOLD).toFloat() );

        m_colormapRenderer->draw( width, height, renderMode );
    }
}

QString DatasetScalar::getValueAsString( int x, int y, int z )
{
    float data = m_data[ getId( x, y, z ) ];
    return QString::number( data );
}

bool DatasetScalar::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    int paintMode = m_properties["maingl"]->get( Fn::Property::D_PAINTMODE ).toInt();
    if ( pickId == 0 ||  paintMode == 0 || !( modifiers & Qt::ControlModifier ) )
    {
        return false;
    }

    float dx = m_properties["maingl"]->get( Fn::Property::D_DX ).toFloat();
    float dy = m_properties["maingl"]->get( Fn::Property::D_DY ).toFloat();
    float dz = m_properties["maingl"]->get( Fn::Property::D_DZ ).toFloat();

    float paintValue = m_properties["maingl"]->get( Fn::Property::D_PAINTVALUE ).toFloat();

    int brushSize = m_properties["maingl"]->get( Fn::Property::D_PAINTSIZE ).toInt() - 1;

    int x = pos.x() / dx;
    int y = pos.y() / dy;
    int z = pos.z() / dz;

    m_data[ getId( x, y, z ) ] = paintValue;

    for ( int i = -brushSize; i <= brushSize; ++i )
    {
       for ( int j = -brushSize; j <= brushSize; ++j )
       {
           for ( int k = -brushSize; k <= brushSize; ++k )
           {
               if ( pow( (float)( i*i + j*j + k*k ), 1.0/3.0 ) <=  (float)brushSize / 2.0 )
               {
                   m_data[ getId( x + i, y + j, z + k ) ] = paintValue;
               }
           }
       }
    }

    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;

    if ( m_properties["maingl"]->get( Fn::Property::D_PAINT_LINK_CURSOR ).toBool() )
    {
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ), pos.x() / dx );
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ), pos.y() / dy );
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ), pos.z() / dz );
    }

    return true;
}

float DatasetScalar::getInterpolatedValueAtPos( QVector3D pos )
{
    float x = pos.x() / m_properties["maingl"]->get( Fn::Property::D_DX ).toFloat();
    float y = pos.y() / m_properties["maingl"]->get( Fn::Property::D_DY ).toFloat();
    float z = pos.z() / m_properties["maingl"]->get( Fn::Property::D_DZ ).toFloat();

    int x0 = (int) x;
    int y0 = (int) y;
    int z0 = (int) z;

    float xd = x - x0;
    float yd = y - y0;
    float zd = z - z0;

    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    int blockSize = nx * ny * nz;

    int id = getIdFromPos( pos.x(), pos.y(), pos.z() );
    int id_x0y0z0 = id;
    int id_x1y0z0 = qMin( blockSize - 1, id + 1 );
    int id_x0y1z0 = qMin( blockSize - 1, id + nx );
    int id_x1y1z0 = qMin( blockSize - 1, id + nx + 1 );
    int id_x0y0z1 = qMin( blockSize - 1, id + nx * ny );
    int id_x1y0z1 = qMin( blockSize - 1, id + nx * ny + 1 );
    int id_x0y1z1 = qMin( blockSize - 1, id + nx * ny + nx );
    int id_x1y1z1 = qMin( blockSize - 1, id + nx * ny + nx + 1 );

    float i1;
    float i2;
    float j1;
    float j2;
    float w1;
    float w2;
    float iv;

    i1 = m_data[ id_x0y0z0 ] * ( 1.0 - zd ) + m_data[ id_x0y0z1 ] * zd;
    i2 = m_data[ id_x0y1z0 ] * ( 1.0 - zd ) + m_data[ id_x0y1z1 ] * zd;
    j1 = m_data[ id_x1y0z0 ] * ( 1.0 - zd ) + m_data[ id_x1y0z1 ] * zd;
    j2 = m_data[ id_x1y1z0 ] * ( 1.0 - zd ) + m_data[ id_x1y1z1 ] * zd;

    w1 = i1 * ( 1.0 - yd ) + i2 * yd;
    w2 = j1 * ( 1.0 - yd ) + j2 * yd;

    iv = w1 * ( 1.0 - xd ) + w2 * xd;

    return iv;
}

QColor DatasetScalar::getColorAtPos( QVector3D pos )
{
    float value = getInterpolatedValueAtPos( pos );

    float colormapID = m_properties["maingl"]->get( Fn::Property::D_COLORMAP ).toInt();
    float min = m_properties["maingl"]->get( Fn::Property::D_SELECTED_MIN ).toFloat();
    float max = m_properties["maingl"]->get( Fn::Property::D_SELECTED_MAX ).toFloat();
    float lower_t = m_properties["maingl"]->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat();
    float upper_t = m_properties["maingl"]->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat();
    float alpha =  m_properties["maingl"]->get( Fn::Property::D_ALPHA ).toFloat();
    QColor c = ColormapFunctions::getColor( colormapID, value, min, max, lower_t, upper_t, alpha );
    return c;
}
