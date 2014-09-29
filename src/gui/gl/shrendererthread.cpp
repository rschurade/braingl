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

SHRendererThread::SHRendererThread( std::vector<ColumnVector>* data,
                                    QMatrix4x4 pMatrix,
                                    QMatrix4x4 mvMatrix,
                                    PropertyGroup& props ) :
    m_data( data ),
    m_pMatrix( pMatrix ),
    m_mvMatrix( mvMatrix )
{
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

    m_scaling = props.get( Fn::Property::D_MINMAX_SCALING ).toFloat();
    m_lod = props.get( Fn::Property::D_LOD ).toInt();
    m_order = props.get( Fn::Property::D_ORDER ).toInt();
    m_hideNegative = props.get( Fn::Property::D_HIDE_NEGATIVE_LOBES ).toBool();
    m_orient = 0;
    m_offset = props.get( Fn::Property::D_OFFSET ).toFloat();
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

    m_verts = new std::vector<float>();
    m_normals = new std::vector<float>();
    m_indices = new std::vector<unsigned int>();
    m_colors = new std::vector<float>();
}

SHRendererThread::~SHRendererThread()
{
    m_indices->clear();
    std::vector<unsigned int>().swap( *m_indices );
    m_verts->clear();
    std::vector<float>().swap( *m_verts );
    m_normals->clear();
    std::vector<float>().swap( *m_normals );
    m_colors->clear();
    std::vector<float>().swap( *m_colors );
}

void SHRendererThread::run()
{
    const Matrix* vertices = tess::vertices( m_lod );
    const int* faces = tess::faces( m_lod );

    int numVerts = tess::n_vertices( m_lod );
    int numTris = tess::n_faces( m_lod );
    Matrix base = ( FMath::sh_base( (*vertices), m_order ) );

    QMatrix4x4 mvp = m_pMatrix * m_mvMatrix;

    int numBalls = 0;

    try
    {
        if ( ( m_orient & 1 ) == 1 )
        {
            int glyphs = m_nx * m_ny;
            //verts->reserve( numVerts * glyphs * 3 );
            QVector4D pos( 0, 0, m_z, 1.0 );
            for( int yy = 0; yy < m_ny; ++yy )
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
                            ++numBalls;
                            ColumnVector r = base * dv;

                            if ( m_scaling )
                            {
                                float max = 0;
                                float min = std::numeric_limits<float>::max();
                                for ( int i = 1; i <= r.Nrows(); ++i )
                                {
                                    max = qMax( max, (float)r( i ) );
                                    min = qMin( min, (float)r( i ) );
                                }


                                for ( int i = 1; i <= r.Nrows(); ++i )
                                {
                                    r( i ) = r( i ) / max * 0.8;
                                }
                            }

                            for ( int i = 1; i <= numVerts; ++i )
                            {
                                if ( r( i ) < 0 && m_hideNegative )
                                {
                                    m_verts->push_back( locX );
                                    m_verts->push_back( locY );
                                    m_verts->push_back( m_z + m_offset );
                                }
                                else
                                {
                                    m_verts->push_back( ( (*vertices)( i, 1 ) ) * r( i ) + locX );
                                    m_verts->push_back( ( (*vertices)( i, 2 ) ) * r( i ) + locY );
                                    m_verts->push_back( ( (*vertices)( i, 3 ) ) * r( i ) + m_z + m_offset );
                                }
                                if ( r(i) > 0 )
                                {
                                    float ro = qMin( 1.0, ( r( i ) / 2. ) + 0.5 );
                                    m_colors->push_back( fabs( (*vertices)( i, 1 ) * ro ) );
                                    m_colors->push_back( fabs( (*vertices)( i, 2 ) * ro ) );
                                    m_colors->push_back( fabs( (*vertices)( i, 3 ) * ro ) );
                                    m_colors->push_back( 1.0 );
                                }
                                else
                                {
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 1.0 );
                                }
                            }
                        }
                    }
                }
            }
        }
        if ( ( m_orient & 2 ) == 2 )
        {
            int glyphs = m_nx * m_nz;
            //verts->reserve( numVerts * glyphs * 3 );
            QVector4D pos( 0, m_y, 0, 1.0 );
            for( int zz = 0; zz < m_nz; ++zz )
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
                            ++numBalls;
                            ColumnVector r = base * dv;

                            if ( m_scaling )
                            {
                                float max = 0;
                                float min = std::numeric_limits<float>::max();
                                for ( int i = 1; i <= r.Nrows(); ++i )
                                {
                                    max = qMax( max, (float)r( i ) );
                                    min = qMin( min, (float)r( i ) );
                                }


                                for ( int i = 1; i <= r.Nrows(); ++i )
                                {
                                    r( i ) = r( i ) / max * 0.8;
                                }
                            }


                            for ( int i = 1; i <= numVerts; ++i )
                            {
                                if ( r( i ) < 0 && m_hideNegative )
                                {
                                    m_verts->push_back( locX );
                                    m_verts->push_back( m_y + m_offset );
                                    m_verts->push_back( locZ );
                                }
                                else
                                {
                                    m_verts->push_back( (*vertices)( i, 1 ) * r( i ) + locX );
                                    m_verts->push_back( (*vertices)( i, 2 ) * r( i ) + m_y + m_offset );
                                    m_verts->push_back( (*vertices)( i, 3 ) * r( i ) + locZ );
                                }
                                if ( r(i) > 0 )
                                {
                                    float ro = qMin( 1.0, ( r( i ) / 2. ) + 0.5 );
                                    m_colors->push_back( fabs( (*vertices)( i, 1 ) * ro ) );
                                    m_colors->push_back( fabs( (*vertices)( i, 2 ) * ro ) );
                                    m_colors->push_back( fabs( (*vertices)( i, 3 ) * ro ) );
                                    m_colors->push_back( 1.0 );
                                }
                                else
                                {
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 1.0 );
                                }
                            }
                        }
                    }
                }
            }
        }
        if ( ( m_orient & 4 ) == 4 )
        {
            int glyphs = m_ny * m_nz;
            //verts->reserve( numVerts * glyphs * 3 );

            QVector4D pos( m_x, 0, 0, 1.0 );
            for( int yy = 0; yy < m_ny; ++yy )
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
                            ++numBalls;
                            ColumnVector r = base * dv;

                            if ( m_scaling )
                            {
                                float max = 0;
                                float min = std::numeric_limits<float>::max();
                                for ( int i = 1; i <= r.Nrows(); ++i )
                                {
                                    max = qMax( max, (float)r( i ) );
                                    min = qMin( min, (float)r( i ) );
                                }


                                for ( int i = 1; i <= r.Nrows(); ++i )
                                {
                                    r( i ) = r( i ) / max * 0.8;
                                }
                            }

                            for ( int i = 1; i <= numVerts; ++i )
                            {
                                if ( r( i ) < 0 && m_hideNegative )
                                {
                                    m_verts->push_back( m_x + m_offset );
                                    m_verts->push_back( locY );
                                    m_verts->push_back( locZ );
                                }
                                else
                                {
                                    m_verts->push_back( (*vertices)( i, 1 ) * r( i ) + m_x + m_offset );
                                    m_verts->push_back( (*vertices)( i, 2 ) * r( i ) + locY );
                                    m_verts->push_back( (*vertices)( i, 3 ) * r( i ) + locZ );
                                }
                                if ( r(i) > 0 )
                                {
                                    float ro = qMin( 1.0, ( r( i ) / 2. ) + 0.5 );
                                    m_colors->push_back( fabs( (*vertices)( i, 1 ) * ro ) );
                                    m_colors->push_back( fabs( (*vertices)( i, 2 ) * ro ) );
                                    m_colors->push_back( fabs( (*vertices)( i, 3 ) * ro ) );
                                    m_colors->push_back( 1.0 );
                                }
                                else
                                {
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 0.5 );
                                    m_colors->push_back( 1.0 );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    catch (std::bad_alloc& ba)
    {
        qCritical() << "bad alloc sh renderer";
        emit( finished( false ) );
        return;
    }

    for ( auto i = 0; i < numBalls; ++i )
    {
        for ( int k = 0; k < numTris; ++k )
        {
            m_indices->push_back( faces[k*3  ] + numVerts * i );
            m_indices->push_back( faces[k*3+2] + numVerts * i );
            m_indices->push_back( faces[k*3+1] + numVerts * i );
        }
    }

    numTris = m_indices->size() / 3;
    std::vector<float>triNormals;

    float v1x, v1y, v1z;
    float v2x, v2y, v2z;

    for ( auto i = 0; i < numTris; ++i )
    {
        v1x = m_verts->at( 3 * m_indices->at( 3 * i + 1 ) )     - m_verts->at( 3 * m_indices->at( 3 * i ) );
        v1y = m_verts->at( 3 * m_indices->at( 3 * i + 1 ) + 1 ) - m_verts->at( 3 * m_indices->at( 3 * i ) + 1 );
        v1z = m_verts->at( 3 * m_indices->at( 3 * i + 1 ) + 2 ) - m_verts->at( 3 * m_indices->at( 3 * i ) + 2 );

        v2x = m_verts->at( 3 * m_indices->at( 3 * i + 2 ) )     - m_verts->at( 3 * m_indices->at( 3 * i ) );
        v2y = m_verts->at( 3 * m_indices->at( 3 * i + 2 ) + 1 ) - m_verts->at( 3 * m_indices->at( 3 * i ) + 1 );
        v2z = m_verts->at( 3 * m_indices->at( 3 * i + 2 ) + 2 ) - m_verts->at( 3 * m_indices->at( 3 * i ) + 2 );

        triNormals.push_back( v1y * v2z - v1z * v2y );
        triNormals.push_back( v1z * v2x - v1x * v2z );
        triNormals.push_back( v1x * v2y - v1y * v2x );
    }

    std::vector<std::vector<float> >vertTriNormals( m_verts->size() / 3 );

    for ( auto i = 0; i < numTris; ++i )
    {
        vertTriNormals[ m_indices->at( 3 * i     ) ].push_back( triNormals[3*i] );
        vertTriNormals[ m_indices->at( 3 * i     ) ].push_back( triNormals[3*i+1] );
        vertTriNormals[ m_indices->at( 3 * i     ) ].push_back( triNormals[3*i+2] );

        vertTriNormals[ m_indices->at( 3 * i + 1 ) ].push_back( triNormals[3*i] );
        vertTriNormals[ m_indices->at( 3 * i + 1 ) ].push_back( triNormals[3*i+1] );
        vertTriNormals[ m_indices->at( 3 * i + 1 ) ].push_back( triNormals[3*i+2] );

        vertTriNormals[ m_indices->at( 3 * i + 2 ) ].push_back( triNormals[3*i] );
        vertTriNormals[ m_indices->at( 3 * i + 2 ) ].push_back( triNormals[3*i+1] );
        vertTriNormals[ m_indices->at( 3 * i + 2 ) ].push_back( triNormals[3*i+2] );
    }


    for( auto i = 0; i < vertTriNormals.size(); ++i )
    {
        float x = 0;
        float y = 0;
        float z = 0;
        int numNorms = vertTriNormals[i].size() / 3;
        for( auto k = 0; k < numNorms; ++k )
        {
            x += vertTriNormals[i][k*3];
            y += vertTriNormals[i][k*3+1];
            z += vertTriNormals[i][k*3+2];
        }
        x /= numNorms;
        y /= numNorms;
        z /= numNorms;

        m_normals->push_back( x );
        m_normals->push_back( y );
        m_normals->push_back( z );

        std::vector<float>().swap( vertTriNormals[i] );
    }

    std::vector<float>().swap( triNormals );
    std::vector<std::vector<float> >().swap( vertTriNormals );

    emit( finished( true ) );
}
