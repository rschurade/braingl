/*
 * shrendererthread.cpp
 *
 * Created on: 27.12.2012
 * @author Ralph Schurade
 */
#include "binghamrendererthread.h"

#include "glfunctions.h"

#include "../../algos/fmath.h"

#include "../../data/models.h"
#include "../../data/mesh/tesselation.h"

#include "math.h"

BinghamRendererThread::BinghamRendererThread( int id,
                                                   std::vector<std::vector<float> >* data,
                                                   QMatrix4x4 pMatrix,
                                                   QMatrix4x4 mvMatrix,
                                                   PropertyGroup& props
                                                                 ) :
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
    m_render1 = props.get( Fn::Property::D_RENDER_FIRST ).toBool();
    m_render2 = props.get( Fn::Property::D_RENDER_SECOND ).toBool();
    m_render3 = props.get( Fn::Property::D_RENDER_THIRD ).toBool();

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

BinghamRendererThread::~BinghamRendererThread()
{
    m_verts->clear();
}

std::vector<float>* BinghamRendererThread::getVerts()
{
    return m_verts;
}

void BinghamRendererThread::run()
{
    const Matrix* vertices = tess::vertices( m_lod );
    int numVerts = tess::n_vertices( m_lod );

    Matrix base = ( FMath::sh_base( (*vertices), m_order ) );

    int numThreads = GLFunctions::idealThreadCount;

    double radius( 0.0 );
    QMatrix4x4 mvp = m_pMatrix * m_mvMatrix;
    if ( ( m_orient & 1 ) == 1 )
    {
        int glyphs = m_nx * m_ny;
        m_verts->reserve( numVerts * glyphs * 7 );
        QVector4D pos( 0, 0, m_z, 1.0 );
        for( int yy = m_id; yy < m_ny; yy += numThreads )
        {
            for ( int xx = 0; xx < m_nx; ++xx )
            {
                int dataPos = xx + yy * m_nx + m_zi * m_nx * m_ny;
                if ( ( fabs( m_data->at( dataPos )[8] ) > 0.0001 ) )
                {
                    float locX = xx * m_dx + m_ax;
                    float locY = yy * m_dy + m_ay;

                    pos.setX( locX );
                    pos.setY( locY );
                    QVector4D test = mvp * pos;

                    if ( fabs( test.x() / 2.0 ) < 1.0 && fabs( test.y() / 2.0 ) < 1.0 )
                    {
                        for ( int k = 0; k < 3; ++k )
                        {
                            if ( k == 0 && m_render1 )
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
                                    m_verts->push_back( locX );
                                    m_verts->push_back( locY );
                                    m_verts->push_back( m_z );
                                    m_verts->push_back( radius );
                                }
                            }
                            if ( k == 1 && m_render2 )
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
                                    m_verts->push_back( locX );
                                    m_verts->push_back( locY );
                                    m_verts->push_back( m_z );
                                    m_verts->push_back( radius );
                                }
                            }
                            if ( k == 2 && m_render3 )
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
                                    m_verts->push_back( locX );
                                    m_verts->push_back( locY );
                                    m_verts->push_back( m_z );
                                    m_verts->push_back( radius );
                                }
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
        m_verts->reserve( numVerts * glyphs * 7 );
        QVector4D pos( 0, m_y, 0, 1.0 );
        for( int zz = m_id; zz < m_nz; zz += numThreads )
        {
            for ( int xx = 0; xx < m_nx; ++xx )
            {
                int dataPos = xx + m_yi * m_nx + zz * m_nx * m_ny;
                if ( ( fabs( m_data->at( dataPos )[8] ) > 0.0001 ) )
                {
                    float locX = xx * m_dx + m_ax;
                    float locZ = zz * m_dz + m_az;

                    pos.setX( locX );
                    pos.setZ( locZ );
                    QVector4D test = mvp * pos;

                    if ( fabs( test.x() / 2.0 ) < 1.0 && fabs( test.y() / 2.0 ) < 1.0 )
                    {
                        for ( int k = 0; k < 3; ++k )
                        {
                            if ( k == 0 && m_render1 )
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
                                    m_verts->push_back( locX );
                                    m_verts->push_back( m_y );
                                    m_verts->push_back( locZ );
                                    m_verts->push_back( radius );
                                }
                            }
                            if ( k == 1 && m_render2 )
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
                                    m_verts->push_back( locX );
                                    m_verts->push_back( m_y );
                                    m_verts->push_back( locZ );
                                    m_verts->push_back( radius );
                                }
                            }
                            if ( k == 2 && m_render3 )
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
                                    m_verts->push_back( locX );
                                    m_verts->push_back( m_y );
                                    m_verts->push_back( locZ );
                                    m_verts->push_back( radius );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if ( ( m_orient & 4 )  == 4 )
    {
        int glyphs = m_ny * m_nz;
        m_verts->reserve( numVerts * glyphs * 10 );
        QVector4D pos( m_x, 0, 0, 1.0 );
        for( int yy = m_id; yy < m_ny; yy += numThreads )
        {
            for ( int zz = 0; zz < m_nz; ++zz )
            {
                int dataPos = m_xi + yy * m_nx + zz * m_nx * m_ny;
                if ( ( fabs( m_data->at( dataPos )[8] ) > 0.0001 ) )
                {
                    float locZ = zz * m_dz + m_az;
                    float locY = yy * m_dy + m_ay;

                    pos.setY( locY );
                    pos.setZ( locZ );
                    QVector4D test = mvp * pos;

                    if ( fabs( test.x() / 2.0 ) < 1.0 && fabs( test.y() / 2.0 ) < 1.0 )
                    {
                        for ( int k = 0; k < 3; ++k )
                        {
                            if ( k == 0 && m_render1 )
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
                                    m_verts->push_back( m_x );
                                    m_verts->push_back( locY );
                                    m_verts->push_back( locZ );
                                    m_verts->push_back( radius );
                                }
                            }
                            if ( k == 1 && m_render2 )
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
                                    m_verts->push_back( m_x );
                                    m_verts->push_back( locY );
                                    m_verts->push_back( locZ );
                                    m_verts->push_back( radius );
                                }
                            }
                            if ( k == 2 && m_render3 )
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
                                    m_verts->push_back( m_x );
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
    }
    else
    {
        return;
    }
}
