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

#include <math.h>
#include <limits>

SHRendererThread::SHRendererThread( int id, QVector<ColumnVector>* data, int nx, int ny, int nz,
                                                                float dx, float dy, float dz,
                                                                int xi, int yi, int zi,
                                                                int lod,
                                                                int order,
                                                                int orient,
                                                                bool scaling,
                                                                bool hideNegative,
                                                                QMatrix4x4 pMatrix,
                                                                QMatrix4x4 mvMatrix
                                                                 ) :
    m_id( id ),
    m_data( data ),
    m_nx( nx ),
    m_ny( ny ),
    m_nz( nz ),
    m_dx( dx ),
    m_dy( dy ),
    m_dz( dz ),
    m_xi( xi ),
    m_yi( yi ),
    m_zi( zi ),
    m_lod( lod ),
    m_order( order ),
    m_orient( orient ),
    m_scaling( scaling ),
    m_hideNegative( hideNegative ),
    m_pMatrix( pMatrix ),
    m_mvMatrix( mvMatrix )
{
    m_verts = new std::vector<float>();
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

    float x = (float)m_xi * m_dx + m_dx / 2.;
    float y = (float)m_yi * m_dy + m_dy / 2.;
    float z = (float)m_zi * m_dz + m_dz / 2.;

    int numThreads = GLFunctions::idealThreadCount;
    QMatrix4x4 mvp = m_pMatrix * m_mvMatrix;

    try
    {
        if ( ( m_orient & 1 ) == 1 )
        {
            int glyphs = m_nx * m_ny;
            m_verts->reserve( numVerts * glyphs * 7 );
            QVector4D pos( 0, 0, z, 1.0 );
            for( int yy = m_id; yy < m_ny; yy += numThreads )
            {
                for ( int xx = 0; xx < m_nx; ++xx )
                {
                    ColumnVector dv = m_data->at( xx + yy * m_nx + m_zi * m_nx * m_ny );
                    if ( ( fabs( dv(1) ) > 0.0001 ) )
                    {
                        float locX = xx * m_dx + m_dx / 2;
                        float locY = yy * m_dy + m_dy / 2;

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
                                m_verts->push_back( z );
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
            QVector4D pos( 0, y, 0, 1.0 );
            for( int zz = m_id; zz < m_nz; zz += numThreads )
            {
                for ( int xx = 0; xx < m_nx; ++xx )
                {
                    ColumnVector dv = m_data->at( xx + m_yi * m_nx + zz * m_nx * m_ny );
                    if ( ( fabs( dv(1) ) > 0.0001 ) )
                    {
                        float locX = xx * m_dx + m_dx / 2;
                        float locZ = zz * m_dz + m_dz / 2;

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
                                m_verts->push_back( y );
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

            QVector4D pos( x, 0, 0, 1.0 );
            for( int yy = m_id; yy < m_ny; yy += numThreads )
            {
                for ( int zz = 0; zz < m_nz; ++zz )
                {
                    ColumnVector dv = m_data->at( m_xi + yy * m_nx + zz * m_nx * m_ny );
                    if ( ( fabs( dv(1) ) > 0.0001 ) )
                    {
                        float locY = yy * m_dy + m_dy / 2;
                        float locZ = zz * m_dz + m_dz / 2;

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
                                m_verts->push_back( x );
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
        qDebug() << "bad alloc sh renderer thread";
    }
}
