/*
 * dataset3d.cpp
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */
#include "dataset3d.h"
#include "../models.h"

#include "../../gui/gl/evrenderer.h"

Dataset3D::Dataset3D( QDir filename, QVector<QVector3D> data, nifti_image* header ) :
    DatasetNifti( filename, Fn::DatasetType::NIFTI_VECTOR, header ),
    m_data( data ),
    m_renderer( 0 )
{
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, -1 );
    m_properties["maingl"]->create( Fn::Property::D_INTERPOLATION, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0, "general" );
    m_properties["maingl"]->create( Fn::Property::D_DIM, 3 );
    m_properties["maingl"]->create( Fn::Property::D_SCALING, 1.0f, 0.0f, 2.0f, "general" );
    m_properties["maingl"]->create( Fn::Property::D_OFFSET, 0.0f, -0.5, 0.5, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_VECTORS_STICKS, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_SAGITTAL, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_CORONAL, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_RENDER_AXIAL, false, "general" );
    m_properties["maingl"]->create( Fn::Property::D_HAS_TEXTURE, true );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_RENDER_VECTORS_STICKS ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( switchRenderSticks() ) );

    examineDataset();

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"]->getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );
}

Dataset3D::~Dataset3D()
{
    m_properties["maingl"]->set( Fn::Property::D_ACTIVE, false );
    glDeleteTextures( 1, &m_textureGLuint );
    m_data.clear();
    m_data.squeeze();

    delete m_renderer;
}

void Dataset3D::examineDataset()
{
    int type = m_properties["maingl"]->get( Fn::Property::D_DATATYPE ).toInt();
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();
    int size = nx * ny * nz * 3;

    if ( type == DT_UNSIGNED_CHAR )
    {
        m_properties["maingl"]->create( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(unsigned char) ) );

        m_properties["maingl"]->create( Fn::Property::D_MIN, 0 );
        m_properties["maingl"]->create( Fn::Property::D_MAX, 255 );
    }

    if ( type == DT_FLOAT )
    {
        m_properties["maingl"]->create( Fn::Property::D_SIZE, static_cast<int>( size * sizeof(float) ) );
        m_properties["maingl"]->create( Fn::Property::D_MIN, -1.0f );
        m_properties["maingl"]->create( Fn::Property::D_MAX, 1.0f );
    }
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, m_properties["maingl"]->get( Fn::Property::D_MIN ).toFloat() );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, m_properties["maingl"]->get( Fn::Property::D_MAX ).toFloat() );

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties["maingl"]->get( Fn::Property::D_NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
        flipX();
    }

    m_properties["maingl"]->create( Fn::Property::D_PAINTMODE, { "off", "paint" }, 0, "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINTSIZE, 2.f, 1.f, 100.f, "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINTCOLOR, QColor( 255, 0, 0 ), "paint" );

}

void Dataset3D::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

    int type = m_properties["maingl"]->get( Fn::Property::D_DATATYPE ).toInt();
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    float div = 1.0;
    if ( type == DT_UNSIGNED_CHAR )
    {
        div = 255.;
    }

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

void Dataset3D::flipX()
{
    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    QVector<QVector3D> newData;

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

    m_header->qto_xyz.m[0][0] = qMax( m_header->qto_xyz.m[0][0], m_header->qto_xyz.m[0][0] * -1.0f );
    m_header->sto_xyz.m[0][0] = qMax( m_header->sto_xyz.m[0][0], m_header->sto_xyz.m[0][0] * -1.0f );
    m_header->qto_xyz.m[0][3] = 0.0;
    m_header->sto_xyz.m[0][3] = 0.0;

    m_data.clear();
    m_data = newData;
}

QVector<QVector3D>* Dataset3D::getData()
{
    return &m_data;
}

void Dataset3D::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    if ( m_renderer == 0 )
    {
        m_renderer = new EVRenderer( &m_data, properties( target )->get( Fn::Property::D_NX ).toInt(),
                                              properties( target )->get( Fn::Property::D_NY ).toInt(),
                                              properties( target )->get( Fn::Property::D_NZ ).toInt(),
                                              properties( target )->get( Fn::Property::D_DX ).toFloat(),
                                              properties( target )->get( Fn::Property::D_DY ).toFloat(),
                                              properties( target )->get( Fn::Property::D_DZ ).toFloat() );
        m_renderer->setModel( Models::g() );
    }

    if ( properties( target )->get( Fn::Property::D_RENDER_VECTORS_STICKS ).toBool() )
    {
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
    int paintMode = m_properties["maingl"]->get( Fn::Property::D_PAINTMODE ).toInt();
    if ( pickId == 0 ||  paintMode == 0 || !( modifiers & Qt::ControlModifier ) )
    {
        return false;
    }

   QColor paintColorC = m_properties["maingl"]->get( Fn::Property::D_PAINTCOLOR ).value<QColor>();
   QVector3D paintValue;
   int type = m_properties["maingl"]->get( Fn::Property::D_DATATYPE ).toInt();
   if ( type == DT_UNSIGNED_CHAR )
   {
       paintValue = QVector3D( paintColorC.red(), paintColorC.green(), paintColorC.blue() );
   }
   else
   {
       paintValue = QVector3D( paintColorC.redF(), paintColorC.greenF(), paintColorC.blueF() );
   }

   float dx = m_properties["maingl"]->get( Fn::Property::D_DX ).toFloat();
   float dy = m_properties["maingl"]->get( Fn::Property::D_DY ).toFloat();
   float dz = m_properties["maingl"]->get( Fn::Property::D_DZ ).toFloat();

   m_data[ getIdFromPos( pos.x(), pos.y(), pos.z() ) ] = paintValue;

   int brushSize = m_properties["maingl"]->get( Fn::Property::D_PAINTSIZE ).toInt();

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
    m_properties["maingl"]->set( Fn::Property::D_HAS_TEXTURE, !( m_properties["maingl"]->get( Fn::Property::D_RENDER_VECTORS_STICKS ).toBool() ) );
    Models::d()->submit();
}
