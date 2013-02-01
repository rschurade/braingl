/*
 * BinghamThread.cpp
 *
 *  Created on: 27.12.2012
 *      Author: Ralph
 */
#include "binghamthread.h"

#include "fmath.h"
#include "sorts.h"

#include "../data/datasets/datasetsh.h"
#include "../data/mesh/tesselation.h"

BinghamThread::BinghamThread( DatasetSH* ds, int lod, int id ) :
    m_ds( ds ),
    m_lod( lod ),
    m_id( id )
{
}

BinghamThread::~BinghamThread()
{
    m_resultVector.clear();
}

QVector<QVector<float> > BinghamThread::getResultVector()
{
    return m_resultVector;
}

void BinghamThread::run()
{
    int numThreads = QThread::idealThreadCount();
    QVector<ColumnVector>* data = m_ds->getData();

    int num_max = 3;
    int neighbourhood = 3;

    const Matrix* vertices = tess::vertices( m_lod );
    const int* faces = tess::faces( m_lod );
    int numVerts = tess::n_vertices( m_lod );
    int numTris = tess::n_faces( m_lod );

    QVector<QSet<int> > neighs;

    QSet<int> v;
    for ( int i = 0; i < numVerts; ++i )
    {
        neighs.push_back( v );
    }

    for ( int i = 0; i < numTris; ++i )
    {
        int v1 = faces[i * 3];
        int v2 = faces[i * 3 + 1];
        int v3 = faces[i * 3 + 2];

        neighs[v1].insert( v2 );
        neighs[v1].insert( v3 );

        neighs[v2].insert( v1 );
        neighs[v2].insert( v3 );

        neighs[v3].insert( v2 );
        neighs[v3].insert( v1 );
    }

    const int order( ( -3 + static_cast<int>( sqrt( 8 * data->at( 0 ).Nrows() + 1 ) ) ) / 2 );
    Matrix base = ( FMath::sh_base( ( *vertices ), order ) );

    QVector<float> bv( 3 * 9, 0 );

    m_resultVector.reserve( data->size() / numThreads + numThreads );

// For all voxels:
    int done = 0;
    for ( int i = m_id ; i < data->size(); i += numThreads )
    {
        {
            QVector<float> v = fit_bingham( data->at( i ), *vertices, neighs, base, neighbourhood, num_max );
            m_resultVector.push_back( v );
            ++done;
        }
    }
}

QVector<float> BinghamThread::fit_bingham( const ColumnVector& sh_data,
                                                  const Matrix& tess,
                                                  const QVector<QSet<int> >& adj,
                                                  const Matrix& base,
                                                  const int neighborhood,
                                                  const int num_max )
{
    unsigned int mod = 9;
    // reserve memory:
    QVector<float> result( 27, 0 );


    // if no CSD no fit necessary.
    if ( sh_data( 1 ) == 0 )
    {
        return result;
    }

    // get maxima:
    ColumnVector radius = base * sh_data;

    QVector<float> qfRadius( radius.Nrows() );
    for ( int i = 0; i < qfRadius.size(); ++i )
    {
        qfRadius[i] = radius( i + 1 );
    }

    QVector<int> qiRadius( radius.Nrows() );
    for ( int i = 0; i < qiRadius.size(); ++i )
    {
        qiRadius[i] = i;
    }

    QVector<int> maxima;
    for ( int i = 0; i < qfRadius.size(); ++i )
    {
        QSet<int> n = adj[i];
        float r = qfRadius[i];
        if ( r > 0 )
        {
            bool isMax = true;
            foreach (const int &value, n)
            {
                if ( r < qfRadius[value] )
                {
                    isMax = false;
                }
            }
            if ( isMax )
            {
                maxima.push_back( i );
            }
        }
    }

    if ( maxima.size() > 2 )
    {
        Sorts::quickSort( maxima, qfRadius, 0, maxima.size() - 1 );
    }

//    qDebug() << "found the following maxima:";
//    for ( int i = 0; i < maxima.size(); ++i )
//    {
//        qDebug() << i << maxima[i] << qfRadius[maxima[i]] << adj[maxima[i]];
//    }

    // For all maxima:
    for ( int n_max = 0; ( n_max < maxima.size() / 2 ) && ( n_max < num_max ); ++n_max )
    {
        // add all maxima and their surrounding points within range of (neighborhood) to the vector gv
        QSet<int> g; // = adj[maxima[2 * n_max]];
        g.insert( maxima[2 * n_max] );
        for ( int neighborIterations = 0; neighborIterations < neighborhood; ++neighborIterations )
        {
            QSet<int> h( g );
            foreach (const int &value, h )
            {
                g.unite( adj[value] );
            }
            g.unite( h );
        }

        QSet<int> g2; // = adj[maxima[2 * n_max + 1]];
        g2.insert( maxima[2 * n_max + 1] );
        for ( int neighborIterations = 0; neighborIterations < neighborhood; ++neighborIterations )
        {
            QSet<int> h( g2 );
            foreach (const int &value, h )
            {
                g2.unite( adj[value] );
            }
            g2.unite( h );
        }
        g.unite( g2 );

        QVector<int> gv = g.toList().toVector();

        // testing if there is a neighbor with a negative value, skipping that maximum if true
        bool negNeigh = false;
        for ( int i = 0; i < gv.size(); ++i )
        {
            if ( qfRadius[gv[i]] < 0 )
            {
                negNeigh = true;
            }
        }
        if ( negNeigh )
        {
            break;
        }

        // sort maxima biggest radi first
        Sorts::quickSort( gv, qfRadius, 0, gv.size() - 1 );

//        qDebug() << "=================================================";
//        for ( int i = 0; i < gv.size(); ++i )
//        {
//            qDebug() << gv[i] << qfRadius[gv[i]];
//        }
//        qDebug() << "=================================================";

        // preprocessing for moment of inertia matrix:
        ColumnVector values( gv.size() );
        QVector<ColumnVector> points;
        ColumnVector maxV( 3 );

        maxV( 1 ) = tess( gv[0] + 1, 1 );
        maxV( 2 ) = tess( gv[0] + 1, 2 );
        maxV( 3 ) = tess( gv[0] + 1, 3 );

        double f0 = qfRadius[gv[0]];

        for ( int i = 0, j = 0; i < gv.size(); ++i )
        {
            ColumnVector cur( 3 );
            cur( 1 ) = tess( gv[i] + 1, 1 );
            cur( 2 ) = tess( gv[i] + 1, 2 );
            cur( 3 ) = tess( gv[i] + 1, 3 );

            double temp = qfRadius[gv[i]];

            if ( temp > 0.0 && temp <= f0 )
            {
                if ( FMath::iprod( cur, maxV ) < 0 )
                {
                    cur = cur * -1;
                }
                points.push_back( cur );
                values( ++j ) = temp;
            }
        }

        // calculate moment_of_inertia and extract values:
        SymmetricMatrix m_inertia( FMath::moment_of_inertia( values, points ) );

        DiagonalMatrix m_inertiaEvals( 3 );
        Matrix m_inertiaEvecs( 3, 3 );
        EigenValues( m_inertia, m_inertiaEvals, m_inertiaEvecs );

        QVector<ColumnVector> vecs;
        ColumnVector vals( 3 );

        for ( int i = 1; i < 4; ++i )
        {
            ColumnVector tmp( 3 );
            tmp( 1 ) = m_inertiaEvecs( 1, 4 - i );
            tmp( 2 ) = m_inertiaEvecs( 2, 4 - i );
            tmp( 3 ) = m_inertiaEvecs( 3, 4 - i );
            vecs.push_back( tmp );
            vals( i ) = m_inertiaEvals( 4 - i );
        }

        // the eigenvectors are the bingham parameter mu:
        //FMath::evd3x3_2( m_inertia, vecs, vals );
        //qDebug() << vecs[0](1) << vecs[0](2) << vecs[0](3);
/*
        maxV = FMath::sphere2cart( FMath::SH_opt_max( FMath::cart2sphere( vecs[0] ), sh_data ) );

        double angle( acos( FMath::iprod( maxV, vecs[0] ) ) );

        ColumnVector ax( FMath::cprod( maxV, vecs[0] ) ); //axis

        Matrix R( FMath::RotationMatrix( angle, ax ) );

        vecs[0] = maxV;
        vecs[1] = R * vecs[1];
        vecs[2] = R * vecs[2];
*/
        // copies of these vectors in spherical coordinates:

        ColumnVector z0( FMath::cart2sphere( vecs[0] ) );
        //ColumnVector z1( FMath::cart2sphere( vecs[1] ) );
        //ColumnVector z2( FMath::cart2sphere( vecs[2] ) );

        // get function value at maximum:
        f0 = FMath::sh_eval( z0, sh_data );

        if ( gv.size() > 2 )
        {
            // reserve calculation variables:
            Matrix A_tmp( gv.size(), 2 );
            ColumnVector b_tmp( gv.size() );
            ColumnVector index( gv.size() );
            index = 0.0;

            int size( 0 );
            // build matrix for least square solution:
            for ( int i = 0; i < gv.size(); ++i )
            {
                ColumnVector cur( 3 );
                cur( 1 ) = tess( gv[i] + 1, 1 );
                cur( 2 ) = tess( gv[i] + 1, 2 );
                cur( 3 ) = tess( gv[i] + 1, 3 );

                double f( radius( gv[i] + 1 ) );
                if ( f0 > f && f > 0.0 )
                {
                    A_tmp( i + 1, 1 ) = -FMath::iprod( vecs[1], cur ) * FMath::iprod( vecs[1], cur );
                    A_tmp( i + 1, 2 ) = -FMath::iprod( vecs[2], cur ) * FMath::iprod( vecs[2], cur );

                    b_tmp( i + 1 ) = log( f / f0 );
                    ++size;
                    index( i + 1 ) = 1.0;
                }
            }

            Matrix A( size, 2 );
            ColumnVector b( size );

            if ( size != gv.size() )
            {
                size = 0;
                for ( int i = 0; i < gv.size(); ++i )
                {
                    if ( index( i + 1 ) != 0.0 )
                    {
                        A( size + 1, 1 ) = A_tmp( i + 1, 1 );
                        A( size + 1, 2 ) = A_tmp( i + 1, 2 );
                        b( size + 1 ) = b_tmp( i + 1 );
                        ++size;
                    }
                }
            }
            else
            {
                A = A_tmp;
                b = b_tmp;
            }

            if ( size > 2 )
            {
                ColumnVector k_s( FMath::pseudoInverse( A ) * b );

                if ( k_s( 1 ) > 0.0 && k_s( 2 ) > 0.0 )
                {

                    // order accordingly:
                    if ( k_s( 1 ) > k_s( 2 ) )
                    {
                        ColumnVector tmp_v( vecs[1] );
                        vecs[1] = vecs[2];
                        vecs[2] = tmp_v;
                        double tmp_d( k_s( 2 ) );
                        k_s( 2 ) = k_s( 1 );
                        k_s( 1 ) = tmp_d;
                    }

                    // format output:
                    result[n_max * mod + 0] = vecs[1]( 1 );
                    result[n_max * mod + 1] = vecs[1]( 2 );
                    result[n_max * mod + 2] = vecs[1]( 3 );

                    result[n_max * mod + 3] = vecs[2]( 1 );
                    result[n_max * mod + 4] = vecs[2]( 2 );
                    result[n_max * mod + 5] = vecs[2]( 3 );

                    result[n_max * mod + 6] = k_s( 1 );
                    result[n_max * mod + 7] = k_s( 2 );

                    result[n_max * mod + 8] = f0;
                }
            }
        }
    }

    return result;
}
