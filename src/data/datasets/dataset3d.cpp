/*
 * dataset3d.cpp
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */
#include "dataset3d.h"

#include "../../gui/gl/evrenderer.h"

Dataset3D::Dataset3D( QString filename, QVector<QVector3D> data, nifti_image* header ) :
        DatasetNifti( filename, Fn::DatasetType::NIFTI_VECTOR, header ), m_data( data ), m_renderer( 0 )
{
    m_properties.set( FNPROP_COLORMAP, (int)Fn::Colormap::NONE );
    m_properties.set( FNPROP_INTERPOLATION, false, true );
    m_properties.set( FNPROP_ALPHA, 1.0f, 0.0, 1.0, true );
    m_properties.set( FNPROP_DIM, 3 );

    m_properties.set( FNPROP_RENDER_SLICE, 1, 1, 3, true );
    m_properties.set( FNPROP_SCALING, 1.0f, 0.0f, 2.0f, true );
    m_properties.set( FNPROP_OFFSET, 0.0f, -0.5, 0.5, true );

    examineDataset();
}

Dataset3D::~Dataset3D()
{
    m_properties.set( FNPROP_ACTIVE, false );
    delete m_renderer;
    m_data.clear();
}

void Dataset3D::examineDataset()
{
    int type = m_properties.get( FNPROP_DATATYPE ).toInt();
    int nx = m_properties.get( FNPROP_NX ).toInt();
    int ny = m_properties.get( FNPROP_NY ).toInt();
    int nz = m_properties.get( FNPROP_NZ ).toInt();
    int size = nx * ny * nz * 3;

    if ( type == DT_UNSIGNED_CHAR )
    {
        m_properties.set( FNPROP_SIZE, static_cast<int>( size * sizeof(unsigned char) ) );

        m_properties.set( FNPROP_MIN, 0 );
        m_properties.set( FNPROP_MAX, 255 );
    }

    if ( type == DT_FLOAT )
    {
        m_properties.set( FNPROP_SIZE, static_cast<int>( size * sizeof(float) ) );
        m_properties.set( FNPROP_MIN, -1.0f );
        m_properties.set( FNPROP_MAX, 1.0f );
    }
    m_properties.set( FNPROP_LOWER_THRESHOLD, m_properties.get( FNPROP_MIN ).toFloat() );
    m_properties.set( FNPROP_UPPER_THRESHOLD, m_properties.get( FNPROP_MAX ).toFloat() );

    if ( m_qform( 1, 1 ) < 0 || m_sform( 1, 1 ) < 0 )
    {
        qDebug() << m_properties.get( FNPROP_NAME ).toString() << ": RADIOLOGICAL orientation detected. Flipping voxels on X-Axis";
        flipX();
    }
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

    int type = m_properties.get( FNPROP_DATATYPE ).toInt();
    int nx = m_properties.get( FNPROP_NX ).toInt();
    int ny = m_properties.get( FNPROP_NY ).toInt();
    int nz = m_properties.get( FNPROP_NZ ).toInt();

    if ( type == DT_UNSIGNED_CHAR )
    {
        float* data = new float[nx * ny * nz * 3];

        int size = nx * ny * nz * 3;
        for ( int i = 0; i < size; ++i )
        {
            data[i] = qMax( data[i], data[i] * -1.0f ) / 255;
        }
        glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_FLOAT, data );
    }

    if ( type == DT_FLOAT )
    {
        int blockSize = nx * ny * nz;
        float* data = new float[blockSize * 3];

        for ( int i = 0; i < blockSize; ++i )
        {
            data[i * 3] = fabs( m_data[i].x() );
            data[i * 3 + 1] = fabs( m_data[i].y() );
            data[i * 3 + 2] = fabs( m_data[i].z() );
        }
        glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_FLOAT, data );
    }
}

void Dataset3D::flipX()
{
    int nx = m_properties.get( FNPROP_NX ).toInt();
    int ny = m_properties.get( FNPROP_NY ).toInt();
    int nz = m_properties.get( FNPROP_NZ ).toInt();

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

void Dataset3D::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new EVRenderer( &m_data, m_properties.get( FNPROP_NX ).toInt(),
                                              m_properties.get( FNPROP_NY ).toInt(),
                                              m_properties.get( FNPROP_NZ ).toInt(),
                                              m_properties.get( FNPROP_DX ).toFloat(),
                                              m_properties.get( FNPROP_DY ).toFloat(),
                                              m_properties.get( FNPROP_DZ ).toFloat() );
        m_renderer->setModel( model );
        m_renderer->init();
    }

    m_renderer->setRenderParams( m_properties.get( FNPROP_SCALING ).toFloat(),
                                 m_properties.get( FNPROP_RENDER_SLICE ).toInt(),
                                 m_properties.get( FNPROP_OFFSET ).toFloat() );

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString Dataset3D::getValueAsString( int x, int y, int z )
{
    int nx = m_properties.get( FNPROP_NX ).toInt();
    int ny = m_properties.get( FNPROP_NY ).toInt();
    QVector3D data = m_data[x + y * nx + z * nx * ny];
    return QString::number( data.x() ) + ", " + QString::number( data.y() ) + ", " + QString::number( data.z() );
}
