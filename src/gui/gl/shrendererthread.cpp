/*
 * shrendererthread.cpp
 *
 *  Created on: 27.12.2012
 *      Author: Ralph Schurade
 */
#include "shrendererthread.h"

#include "../../algos/fmath.h"
#include "../../data/mesh/tesselation.h"

#include <math.h>
#include <limits>

SHRendererThread::SHRendererThread( QVector<ColumnVector>* data, int nx, int ny, int nz,
                                                                float dx, float dy, float dz,
                                                                int xi, int yi, int zi,
                                                                QVector<int> visibleArea,
                                                                int lod,
                                                                int order,
                                                                int orient,
                                                                bool scaling,
                                                                int id ) :
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
    m_visibleArea( visibleArea ),
    m_lod( lod ),
    m_order( order ),
    m_orient( orient ),
    m_scaling( scaling ),
    m_id( id )
{
    m_verts = new QVector<float>();
}

SHRendererThread::~SHRendererThread()
{
    m_verts->clear();
}

QVector<float>* SHRendererThread::getVerts()
{
    return m_verts;
}

void SHRendererThread::run()
{
    // TODO
    const Matrix* vertices = tess::vertices( m_lod );
    //const int* faces = tess::faces( m_lod );
    int numVerts = tess::n_vertices( m_lod );
    //int numTris = tess::n_faces( m_lod );

    Matrix base = ( FMath::sh_base( (*vertices), m_order ) );

    int lowerX = m_visibleArea[0];
    int lowerY = m_visibleArea[2];
    int lowerZ = m_visibleArea[4];
    int upperX = m_visibleArea[1];
    int upperY = m_visibleArea[3];
    int upperZ = m_visibleArea[5];

    float x = (float)m_xi * m_dx + m_dx / 2.;
    float y = (float)m_yi * m_dy + m_dy / 2.;
    float z = (float)m_zi * m_dz + m_dz / 2. + 0.5;

    int numThreads = QThread::idealThreadCount();

    if ( m_orient == 1 )
    {
        int glyphs = ( upperX - lowerX ) * ( upperY - lowerY );
        m_verts->reserve( numVerts * glyphs * 10 );

        for( int yy = lowerY + m_id; yy < upperY; yy += numThreads )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                if ( ( fabs( m_data->at( xx + yy * m_nx + m_zi * m_nx * m_ny )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = m_data->at( xx + yy * m_nx + m_zi * m_nx * m_ny );
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
                    float locX = xx * m_dx + m_dx / 2;
                    float locY = yy * m_dy + m_dy / 2;

                    for ( int i = 0; i < numVerts; ++i )
                    {
                        m_verts->push_back( (*vertices)( i+1, 1 ) );
                        m_verts->push_back( (*vertices)( i+1, 2 ) );
                        m_verts->push_back( (*vertices)( i+1, 3 ) );
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
    else if ( m_orient == 2 )
    {
        int glyphs = ( upperX - lowerX ) * ( upperY - lowerY );
        m_verts->reserve( numVerts * glyphs * 10 );

        for( int zz = lowerZ + m_id; zz < upperZ; zz += numThreads )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                if ( ( fabs( m_data->at( xx + m_yi * m_nx + zz * m_nx * m_ny )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = m_data->at( xx + m_yi * m_nx + zz * m_nx * m_ny );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }

                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        r(i+1) = r(i+1) / max;
                    }

                    float locX = xx * m_dx + m_dx / 2;
                    float locZ = zz * m_dz + m_dz / 2;

                    for ( int i = 0; i < numVerts; ++i )
                    {
                        m_verts->push_back( (*vertices)( i+1, 1 ) );
                        m_verts->push_back( (*vertices)( i+1, 2 ) );
                        m_verts->push_back( (*vertices)( i+1, 3 ) );
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
    else if ( m_orient == 3 )
    {
        int glyphs = ( upperX - lowerX ) * ( upperY - lowerY );
        m_verts->reserve( numVerts * glyphs * 10 );

        for( int yy = lowerY + m_id; yy < upperY; yy += numThreads )
        {
            for ( int zz = lowerZ; zz < upperZ; ++zz )
            {
                if ( ( fabs( m_data->at( m_xi + yy * m_nx + zz * m_nx * m_ny )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = m_data->at( m_xi + yy * m_nx + zz * m_nx * m_ny );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }

                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        r(i+1) = r(i+1) / max;
                    }

                    float locY = yy * m_dy + m_dy / 2;
                    float locZ = zz * m_dz + m_dz / 2;

                    for ( int i = 0; i < numVerts; ++i )
                    {
                        m_verts->push_back( (*vertices)( i+1, 1 ) );
                        m_verts->push_back( (*vertices)( i+1, 2 ) );
                        m_verts->push_back( (*vertices)( i+1, 3 ) );
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
    else
    {
        return;
    }
}
