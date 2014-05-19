/*
 * dataset3d.cpp
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */
#include "dataset3d.h"
#include "datasetscalar.h"
#include "../models.h"

#include "../../gui/gl/evrenderer.h"

#include "../properties/propertyselection.h"

Dataset3D::Dataset3D( QDir filename, std::vector<QVector3D> data, nifti_image* header ) :
    DatasetNifti( filename, Fn::DatasetType::NIFTI_VECTOR, header ),
    m_data( data ),
    m_renderer( 0 )
{
    m_properties["maingl"].createInt( Fn::Property::D_COLORMAP, -1 );
    m_properties["maingl"].createBool( Fn::Property::D_INTERPOLATION, false, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0, "general" );
    m_properties["maingl"].createInt( Fn::Property::D_DIM, 3 );
    m_properties["maingl"].createFloat( Fn::Property::D_SCALING, 1.0f, 0.0f, 2.0f, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_OFFSET, 0.0f, -0.5, 0.5, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_VECTORS_STICKS, true, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_CORONAL, false, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_AXIAL, true, "general" );
    m_properties["maingl"].createBool( Fn::Property::D_HAS_TEXTURE, false );

    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_VECTORS_STICKS ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( switchRenderSticks() ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_SCALING ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( scalingChanged() ) );

    m_properties["maingl"].createBool( Fn::Property::D_RENDER_VECTORS_STIPPLES, false, "stipples" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_RENDER_VECTORS_STIPPLES ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( switchRenderStipples() ) );
    m_properties["maingl"].createList( Fn::Property::D_STIPPLE_PROB_MASK, { "none" }, 0, "stipples" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_STIPPLE_PROB_MASK ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( probMaskChanged() ) );
    m_properties["maingl"].createColor( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), "stipples" );
    m_properties["maingl"].createInt( Fn::Property::D_LINE_WIDTH, 2, 1, 5, "stipples" );

    examineDataset();

    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"].getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );

    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( updateMaskSelect() ) );
    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( updateMaskSelect() ) );

    updateMaskSelect();
    probMaskChanged();
}

Dataset3D::~Dataset3D()
{
    m_properties["maingl"].set( Fn::Property::D_ACTIVE, false );
    glDeleteTextures( 1, &m_textureGLuint );
    m_data.clear();

    delete m_renderer;
}

void Dataset3D::examineDataset()
{
    int type = m_properties["maingl"].get( Fn::Property::D_DATATYPE ).toInt();
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();
    int size = nx * ny * nz * 3;

    if ( type == DT_UNSIGNED_CHAR )
    {
        m_properties["maingl"].createInt( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(unsigned char) ) );

        m_properties["maingl"].createInt( Fn::Property::D_MIN, 0 );
        m_properties["maingl"].createInt( Fn::Property::D_MAX, 255 );
    }

    if ( type == DT_FLOAT )
    {
        m_properties["maingl"].createInt( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(float) ) );
        m_properties["maingl"].createFloat( Fn::Property::D_MIN, -1.0f );
        m_properties["maingl"].createFloat( Fn::Property::D_MAX, 1.0f );
    }
    m_properties["maingl"].createFloat( Fn::Property::D_LOWER_THRESHOLD, m_properties["maingl"].get( Fn::Property::D_MIN ).toFloat() );
    m_properties["maingl"].createFloat( Fn::Property::D_UPPER_THRESHOLD, m_properties["maingl"].get( Fn::Property::D_MAX ).toFloat() );

    m_properties["maingl"].createList( Fn::Property::D_PAINTMODE, { "off", "paint" }, 0, "paint" );
    m_properties["maingl"].createFloat( Fn::Property::D_PAINTSIZE, 2.f, 1.f, 100.f, "paint" );
    m_properties["maingl"].createColor( Fn::Property::D_PAINTCOLOR, QColor( 255, 0, 0 ), "paint" );

}

void Dataset3D::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, /*GL_CLAMP*/ GL_CLAMP_TO_EDGE );    // XXX CoreProfile
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, /*GL_CLAMP*/ GL_CLAMP_TO_EDGE );    // XXX CoreProfile
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, /*GL_CLAMP*/ GL_CLAMP_TO_EDGE );    // XXX CoreProfile

    int type = m_properties["maingl"].get( Fn::Property::D_DATATYPE ).toInt();
    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    float div = 1.0;
    if ( type == DT_UNSIGNED_CHAR )
    {
        div = 255.;
    }
    //div = 255.;
    div *= m_properties["maingl"].get( Fn::Property::D_SCALING ).toFloat();


    int blockSize = nx * ny * nz;
    float* data = new float[blockSize * 3];

    for ( int i = 0; i < blockSize; ++i )
    {
        data[i * 3] = qMin( 1.0, fabs( m_data[i].x() ) / div );
        data[i * 3 + 1] = qMin( 1.0, fabs( m_data[i].y() ) / div );
        data[i * 3 + 2] = qMin( 1.0, fabs( m_data[i].z() ) / div );
    }
    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_FLOAT, data );

}

std::vector<QVector3D>* Dataset3D::getData()
{
    return &m_data;
}

void Dataset3D::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    if ( m_renderer == 0 )
    {
        m_renderer = new EVRenderer( &m_data );
    }

    if ( properties( target ).get( Fn::Property::D_RENDER_VECTORS_STICKS ).toBool() )
    {
        probMaskChanged();
        m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
}

QString Dataset3D::getValueAsString( int x, int y, int z )
{
    QVector3D data = m_data[ getId( x, y, z ) ];

    return QString::number( data.x() ) + ", " + QString::number( data.y() ) + ", " + QString::number( data.z() );
}

bool Dataset3D::mousePick( int pickId, QVector3D pos,  Qt::KeyboardModifiers modifiers, QString target )
{
    int paintMode = m_properties["maingl"].get( Fn::Property::D_PAINTMODE ).toInt();
    if ( pickId == 0 ||  paintMode == 0 || !( modifiers & Qt::ControlModifier ) )
    {
        return false;
    }

   QColor paintColorC = m_properties["maingl"].get( Fn::Property::D_PAINTCOLOR ).value<QColor>();
   QVector3D paintValue;
   int type = m_properties["maingl"].get( Fn::Property::D_DATATYPE ).toInt();
   if ( type == DT_UNSIGNED_CHAR )
   {
       paintValue = QVector3D( paintColorC.red(), paintColorC.green(), paintColorC.blue() );
   }
   else
   {
       paintValue = QVector3D( paintColorC.redF(), paintColorC.greenF(), paintColorC.blueF() );
   }

   float dx = m_properties["maingl"].get( Fn::Property::D_DX ).toFloat();
   float dy = m_properties["maingl"].get( Fn::Property::D_DY ).toFloat();
   float dz = m_properties["maingl"].get( Fn::Property::D_DZ ).toFloat();

   m_data[ getIdFromPos( pos.x(), pos.y(), pos.z() ) ] = paintValue;

   int brushSize = m_properties["maingl"].get( Fn::Property::D_PAINTSIZE ).toInt();

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
   return true;
}

void Dataset3D::switchRenderSticks()
{
    m_properties["maingl"].set( Fn::Property::D_HAS_TEXTURE, !( m_properties["maingl"].get( Fn::Property::D_RENDER_VECTORS_STICKS ).toBool() ) );
    Models::d()->submit();
}

void Dataset3D::switchRenderStipples()
{
    m_properties["maingl"].set( Fn::Property::D_HAS_TEXTURE, !( m_properties["maingl"].get( Fn::Property::D_RENDER_VECTORS_STICKS ).toBool() ) );
    Models::d()->submit();
}


void Dataset3D::scalingChanged()
{
    glDeleteTextures( 1, &m_textureGLuint );
    createTexture();
}

void Dataset3D::updateMaskSelect()
{
    QList<Dataset*>dsl1 = Models::getDatasets( Fn::DatasetType::NIFTI_SCALAR, false );

    if( dsl1.size() != m_scalarDSL.size() )
    {
        QWidget* widget = m_properties["maingl"].getWidget( Fn::Property::D_STIPPLE_PROB_MASK );
        widget->blockSignals( true );

        int curScalar = m_properties["maingl"].get( Fn::Property::D_STIPPLE_PROB_MASK ).toInt();

        PropertySelection* prop = static_cast<PropertySelection*> ( m_properties["maingl"].getProperty( Fn::Property::D_STIPPLE_PROB_MASK ) );
        prop->clear();

        for ( int i = 0; i < dsl1.size(); ++i )
        {
            prop->addOption( dsl1[i]->properties().get( Fn::Property::D_NAME ).toString() );
        }

        if ( dsl1.size() > m_scalarDSL.size() )
        {
            m_properties["maingl"].set( Fn::Property::D_STIPPLE_PROB_MASK, qMax( 0, curScalar ) );
        }
        else
        {
            m_properties["maingl"].set( Fn::Property::D_STIPPLE_PROB_MASK, 0 );
        }
        m_scalarDSL.clear();
        for ( int i = 0; i < dsl1.size(); ++i )
        {
            m_scalarDSL.push_back( static_cast<DatasetScalar*>( dsl1[i] ) );
        }
        widget->blockSignals( false );

    }
}

void Dataset3D::probMaskChanged()
{
    if ( m_renderer )
    {
        m_renderer->setMask( m_scalarDSL[ m_properties["maingl"].get( Fn::Property::D_STIPPLE_PROB_MASK ).toInt() ] );
    }
}
