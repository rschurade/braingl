/*
 * shrendererthread.cpp
 *
 *  Created on: 27.12.2012
 *      Author: Ralph Schurade
 */
#include "binghamrendererthread.h"

#include "../../algos/fmath.h"

#include "../../data/mesh/tesselation.h"

#include "math.h"

BinghamRendererThread::BinghamRendererThread( QVector<QVector<float> >* data, int nx, int ny, int nz,
                                                                float dx, float dy, float dz,
                                                                int xi, int yi, int zi,
                                                                QVector<int> visibleArea,
                                                                int lod,
                                                                int order,
                                                                int orient,
                                                                bool scaling,
                                                                int renderPeaks,
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
    m_renderPeaks( renderPeaks ),
    m_id( id )
{
    m_verts = new QVector<float>();
}

BinghamRendererThread::~BinghamRendererThread()
{
    m_verts->clear();
}

QVector<float>* BinghamRendererThread::getVerts()
{
    return m_verts;
}

void BinghamRendererThread::run()
{
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
    double radius( 0.0 );
    if ( m_orient == 1 )
    {
        int glyphs = ( upperX - lowerX ) * ( upperY - lowerY );
        m_verts->reserve( numVerts * glyphs * 10 );

        for( int yy = lowerY + m_id; yy < upperY; yy += numThreads )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                int dataPos = xx + yy * m_nx + m_zi * m_nx * m_ny;
                if ( ( fabs( m_data->at( dataPos )[8] ) > 0.0001 ) )
                {
                    float locX = xx * m_dx + m_dx / 2;
                    float locY = yy * m_dy + m_dy / 2;

                    for ( int k = 0; k < 3; ++k )
                    {
                        if ( k == 0 && ( m_renderPeaks & 1 ) == 1 )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( locY );
                                m_verts->push_back( z );
                                m_verts->push_back( radius );
                            }
                        }
                        if ( k == 1 && ( ( m_renderPeaks & 2 ) == 2 ) )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( locY );
                                m_verts->push_back( z );
                                m_verts->push_back( radius );
                            }
                        }
                        if ( k == 2 && ( m_renderPeaks & 4 ) == 4 )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( locY );
                                m_verts->push_back( z );
                                m_verts->push_back( radius );
                            }
                        }
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
                int dataPos = xx + m_yi * m_nx + zz * m_nx * m_ny;
                if ( ( fabs( m_data->at( dataPos )[8] ) > 0.0001 ) )
                {
                    float locX = xx * m_dx + m_dx / 2;
                    float locZ = zz * m_dz + m_dz / 2;

                    for ( int k = 0; k < 3; ++k )
                    {
                        if ( k == 0 && ( m_renderPeaks & 1 ) == 1 )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( y );
                                m_verts->push_back( locZ );
                                m_verts->push_back( radius );
                            }
                        }
                        if ( k == 1 && ( ( m_renderPeaks & 2 ) == 2 ) )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( y );
                                m_verts->push_back( locZ );
                                m_verts->push_back( radius );
                            }
                        }
                        if ( k == 2 && ( m_renderPeaks & 4 ) == 4 )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( locX );
                                m_verts->push_back( y );
                                m_verts->push_back( locZ );
                                m_verts->push_back( radius );
                            }
                        }
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
                int dataPos = m_xi + yy * m_nx + zz * m_nx * m_ny;
                if ( ( fabs( m_data->at( dataPos )[8] ) > 0.0001 ) )
                {
                    float locZ = zz * m_dz + m_dz / 2;
                    float locY = yy * m_dy + m_dy / 2;

                    for ( int k = 0; k < 3; ++k )
                    {
                        if ( k == 0 && ( m_renderPeaks & 1 ) == 1 )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( x );
                                m_verts->push_back( locY );
                                m_verts->push_back( locZ );
                                m_verts->push_back( radius );
                            }
                        }
                        if ( k == 1 && ( ( m_renderPeaks & 2 ) == 2 ) )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( x );
                                m_verts->push_back( locY );
                                m_verts->push_back( locZ );
                                m_verts->push_back( radius );
                            }
                        }
                        if ( k == 2 && ( m_renderPeaks & 4 ) == 4 )
                        {
                            for( int i = 0; i < numVerts; ++i )
                            {
                                ColumnVector cur( 3 );
                                cur( 1 ) = (*vertices)( i+1, 1 );
                                cur( 2 ) = (*vertices)( i+1, 2 );
                                cur( 3 ) = (*vertices)( i+1, 3 );

                                ColumnVector m1( 3 );
                                m1( 1 ) = m_data->at( dataPos )[k*9];
                                m1( 2 ) = m_data->at( dataPos )[k*9+1];
                                m1( 3 ) = m_data->at( dataPos )[k*9+2];

                                ColumnVector m2( 3 );
                                m2( 1 ) = m_data->at( dataPos )[k*9+3];
                                m2( 2 ) = m_data->at( dataPos )[k*9+4];
                                m2( 3 ) = m_data->at( dataPos )[k*9+5];

                                double val_1( FMath::iprod( m1, cur ) );
                                double val_2( FMath::iprod( m2, cur ) );


                                double k1 = m_data->at( dataPos )[k*9+6];
                                double k2 = m_data->at( dataPos )[k*9+7];
                                double f0 = m_data->at( dataPos )[k*9+8];

                                radius =  f0 * exp( -( k1 * val_1 * val_1 + k2 * val_2 * val_2 ) ) ;

                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( (*vertices)( i+1, 1 ) );
                                m_verts->push_back( (*vertices)( i+1, 2 ) );
                                m_verts->push_back( (*vertices)( i+1, 3 ) );
                                m_verts->push_back( x );
                                m_verts->push_back( locY );
                                m_verts->push_back( locZ );
                                m_verts->push_back( radius );
                            }
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
