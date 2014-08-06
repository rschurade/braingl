/*
 * shrendererthread.cpp
 *
 * Created on: 27.12.2012
 * @author Ralph Schurade
 */
#include "shrendererthread.h"

#include "glfunctions.h"

#include "../../algos/fmath.h"
#include "../../data/mesh/tesselation.h"
#include "../../data/models.h"

#include <math.h>
#include <limits>

SHRendererThread::SHRendererThread( int id,
                                        std::vector<ColumnVector>* data,
                                        QMatrix4x4 pMatrix,
                                        QMatrix4x4 mvMatrix,
                                        PropertyGroup& props ) :
    m_id( id ),
    m_data( data ),
    m_pMatrix( pMatrix ),
    m_mvMatrix( mvMatrix )
{
    m_verts = new std::vector<float>();

    m_x = Models::getGlobal( Fn::Property::G_SAGITTAL ).toFloat();
    m_y = Models::getGlobal( Fn::Property::G_CORONAL ).toFloat();
    m_z = Models::getGlobal( Fn::Property::G_AXIAL ).toFloat();

    m_nx = props.get( Fn::Property::D_NX ).toFloat();
    m_ny = props.get( Fn::Property::D_NY ).toFloat();
    m_nz = props.get( Fn::Property::D_NZ ).toFloat();

    m_dx = props.get( Fn::Property::D_DX ).toFloat();
    m_dy = props.get( Fn::Property::D_DY ).toFloat();
    m_dz = props.get( Fn::Property::D_DZ ).toFloat();

    m_ax = props.get( Fn::Property::D_ADJUST_X ).toFloat();
    m_ay = props.get( Fn::Property::D_ADJUST_Y ).toFloat();
    m_az = props.get( Fn::Property::D_ADJUST_Z ).toFloat();

    m_xi = qMax( 0.0f, qMin( ( m_x + m_dx / 2 - m_ax ) / m_dx, m_nx - 1 ) );
    m_yi = qMax( 0.0f, qMin( ( m_y + m_dy / 2 - m_ay ) / m_dy, m_ny - 1 ) );
    m_zi = qMax( 0.0f, qMin( ( m_z + m_dz / 2 - m_az ) / m_dz, m_nz - 1 ) );

    m_scaling = props.get( Fn::Property::D_SCALING ).toFloat();
    m_lod = props.get( Fn::Property::D_LOD ).toInt();
    m_order = props.get( Fn::Property::D_ORDER ).toInt();
    m_hideNegative = props.get( Fn::Property::D_HIDE_NEGATIVE_LOBES ).toBool();
    m_orient = 0;
    if ( props.get( Fn::Property::D_RENDER_AXIAL ).toBool() )
    {
        m_orient = 1;
    }
    if ( props.get( Fn::Property::D_RENDER_CORONAL ).toBool() )
    {
        m_orient += 2;
    }
    if ( props.get( Fn::Property::D_RENDER_SAGITTAL ).toBool() )
    {
        m_orient += 4;
    }
}

SHRendererThread::~SHRendererThread()
{
    m_verts->clear();
}

std::vector<float>* SHRendererThread::getVerts()
{
    return m_verts;
}

void SHRendererThread::run()
{
    const Matrix* vertices = tess::vertices( m_lod );
    int numVerts = tess::n_vertices( m_lod );

    Matrix base = ( FMath::sh_base( (*vertices), m_order ) );

    int numThreads = GLFunctions::idealThreadCount;
    QMatrix4x4 mvp = m_pMatrix * m_mvMatrix;

    try
    {
        if ( ( m_orient & 1 ) == 1 )
        {
            int glyphs = m_nx * m_ny;
            m_verts->reserve( numVerts * glyphs * 7 );
            QVector4D pos( 0, 0, m_z, 1.0 );
            for( int yy = m_id; yy < m_ny; yy += numThreads )
            {
                for ( int xx = 0; xx < m_nx; ++xx )
                {
                    ColumnVector dv = m_data->at( xx + yy * m_nx + m_zi * m_nx * m_ny );
                    if ( ( fabs( dv(1) ) > 0.0001 ) )
                    {
                        float locX = xx * m_dx + m_ax;
                        float locY = yy * m_dy + m_ay;

                        pos.setX( locX );
                        pos.setY( locY );
                        QVector4D test = mvp * pos;

                        if ( fabs( test.x() / 2.0 ) < 1.0 && fabs( test.y() / 2.0 ) < 1.0 )
                        {
                            ColumnVector r = base * dv;

                            if ( m_scaling )
                            {
                                float max = 0;
                                float min = std::numeric_limits<float>::max();
                                for ( int i = 0; i < r.Nrows(); ++i )
                                {
                                    max = qMax( max, (float)r(i+1) );
                                    min = qMin( min, (float)r(i+1) );
                                }


                                for ( int i = 0; i < r.Nrows(); ++i )
                                {
                                    r(i+1) = r(i+1) / max * 0.8;
                                }
                            }

                            for ( int i = 0; i < numVerts; ++i )
                            {
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( locY );
                                m_verts->push_back( m_z );
                                m_verts->push_back( r(i + 1) );
                            }
                        }
                    }
                }
            }
        }
        if ( ( m_orient & 2 ) == 2 )
        {
            int glyphs = m_nx * m_nz;
            m_verts->reserve( numVerts * glyphs * 7 );
            QVector4D pos( 0, m_y, 0, 1.0 );
            for( int zz = m_id; zz < m_nz; zz += numThreads )
            {
                for ( int xx = 0; xx < m_nx; ++xx )
                {
                    ColumnVector dv = m_data->at( xx + m_yi * m_nx + zz * m_nx * m_ny );
                    if ( ( fabs( dv(1) ) > 0.0001 ) )
                    {
                        float locX = xx * m_dx + m_ax;
                        float locZ = zz * m_dz + m_az;

                        pos.setX( locX );
                        pos.setZ( locZ );
                        QVector4D test = mvp * pos;

                        if ( fabs( test.x() / 2.0 ) < 1.0 && fabs( test.y() / 2.0 ) < 1.0 )
                        {
                            ColumnVector r = base * dv;

                            if ( m_scaling )
                            {
                                float max = 0;
                                float min = std::numeric_limits<float>::max();
                                for ( int i = 0; i < r.Nrows(); ++i )
                                {
                                    max = qMax( max, (float)r(i+1) );
                                    min = qMin( min, (float)r(i+1) );
                                }


                                for ( int i = 0; i < r.Nrows(); ++i )
                                {
                                    r(i+1) = r(i+1) / max * 0.8;
                                }
                            }


                            for ( int i = 0; i < numVerts; ++i )
                            {
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( m_y );
                                m_verts->push_back( locZ );
                                m_verts->push_back( r(i + 1) );
                            }
                        }
                    }
                }
            }
        }
        if ( ( m_orient & 4 ) == 4 )
        {
            int glyphs = m_ny * m_nz;
            m_verts->reserve( numVerts * glyphs * 7 );

            QVector4D pos( m_x, 0, 0, 1.0 );
            for( int yy = m_id; yy < m_ny; yy += numThreads )
            {
                for ( int zz = 0; zz < m_nz; ++zz )
                {
                    ColumnVector dv = m_data->at( m_xi + yy * m_nx + zz * m_nx * m_ny );
                    if ( ( fabs( dv(1) ) > 0.0001 ) )
                    {
                        float locY = yy * m_dy + m_ay;
                        float locZ = zz * m_dz + m_az;

                        pos.setY( locY );
                        pos.setZ( locZ );
                        QVector4D test = mvp * pos;

                        if ( fabs( test.x() / 2.0 ) < 1.0 && fabs( test.y() / 2.0 ) < 1.0 )
                        {
                            ColumnVector r = base * dv;

                            if ( m_scaling )
                            {
                                float max = 0;
                                float min = std::numeric_limits<float>::max();
                                for ( int i = 0; i < r.Nrows(); ++i )
                                {
                                    max = qMax( max, (float)r(i+1) );
                                    min = qMin( min, (float)r(i+1) );
                                }


                                for ( int i = 0; i < r.Nrows(); ++i )
                                {
                                    r(i+1) = r(i+1) / max * 0.8;
                                }
                            }

                            for ( int i = 0; i < numVerts; ++i )
                            {
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( m_x );
                                m_verts->push_back( locY );
                                m_verts->push_back( locZ );
                                m_verts->push_back( r(i + 1) );
                            }
                        }
                    }
                }
            }
        }
        else
        {
            return;
        }
    }
    catch (std::bad_alloc& ba)
    {
        qCritical() << "bad alloc sh renderer thread";
    }
}
