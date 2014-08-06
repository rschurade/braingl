/*
 * bingham.cpp
 *
 * Created on: Nov 23, 2012
 * @author Ralph Schurade
 */
#include "bingham.h"
#include "binghamthread.h"
#include "fmath.h"
#include "gradients.h"
#include "sorts.h"

#include "../io/writer.h"

#include "../data/datasets/dataset.h"
#include "../data/datasets/datasetdwi.h"
#include "../data/datasets/datasetsh.h"
#include "../data/datasets/datasetbingham.h"
#include "../data/datasets/datasettensor.h"
#include "../data/mesh/tesselation.h"

#include "../gui/gl/glfunctions.h"

#include "boost/math/special_functions/spherical_harmonic.hpp"

#include <QDebug>

Bingham::Bingham()
{
}

Bingham::~Bingham()
{
}

QList<Dataset*> Bingham::calc_bingham( DatasetSH* sh, const int lod, const int neighbourhood, const int num_max )
{
    QList<Dataset*> dsout;
    std::vector<ColumnVector>* data = sh->getData();

    const int order( ( -3 + static_cast<int>( sqrt( 8 * data->at( 0 ).Nrows() + 1 ) ) ) / 2 );
    qDebug() << "calculated order from sh: " << order;

    int numThreads = GLFunctions::idealThreadCount;

    std::vector<BinghamThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new BinghamThread( sh, lod, i ) );
    }

    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->wait();
    }

    std::vector<std::vector<float> > out;
    out.resize( data->size() );

    // combine fibs from all threads
    qDebug() << "combining results";
    std::vector<float> v( 27, 0 );
    for ( unsigned int i = 0; i < data->size(); ++i )
    {
        out[i] = v;
    }


    for ( int i = 0; i < numThreads; ++i )
    {
        std::vector<std::vector<float> > result = threads[i]->getResultVector();
        qDebug() << i << result.size();
        for ( unsigned int k = 0, l = i; k < result.size(); ++k, l += numThreads )
        {
            out[l] = result.at( k);
        }
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }

    Writer writer( sh, QFileInfo() );
    DatasetBingham* out1 = new DatasetBingham( QDir( "Bingham" ), out, writer.createHeader( 27 ) );
    dsout.push_back( out1 );

    return dsout;
}

QList<Dataset*> Bingham::bingham2Tensor( DatasetBingham* ds )
{
    qDebug() << "start bingham to dwi";
    std::vector< std::vector<float> >* data = ds->getData();

    std::vector<ColumnVector> signals1;
    std::vector<ColumnVector> signals2;
    std::vector<ColumnVector> signals3;

    signals1.resize( data->size() );
    signals2.resize( data->size() );
    signals3.resize( data->size() );

    float kernel[4] = { 258.747, -82.5396, 18.716, -2.35225 };

    int gradientsSize = __GRADIENTS_60_SIZE;
    int tessGradientsSize = tess::__TESS_THREE_V_SIZE;

    ColumnVector v( gradientsSize );
    v = 0.0;
    for ( unsigned int i = 0; i < data->size(); ++i )
    {
        signals1[i] = v;
        signals2[i] = v;
        signals3[i] = v;
    }

    std::vector< std::vector<ColumnVector>* > sigs;
    sigs.push_back( &signals1 );
    sigs.push_back( &signals2 );
    sigs.push_back( &signals3 );

    ColumnVector m1( 3 );
    ColumnVector m2( 3 );
    float k1;
    float k2;
    float f0;

    //****************************************************************************
    std::vector<QVector3D>bvecs;
    std::vector<float>bvals;

    const int max_ord = 10; //((-3+static_cast<int>(sqrt(8*((*sh.get(0,0,0)).size())+1)))/2);


    Matrix gradients( gradientsSize, 3 );
    Matrix tessGradients( tessGradientsSize, 3 );

    for ( int i = 0; i < gradientsSize; ++i )
    {
        gradients( i + 1, 1 ) = __GRADIENTS_60[ i * 3 ];
        gradients( i + 1, 2 ) = __GRADIENTS_60[ i * 3 + 1 ];
        gradients( i + 1, 3 ) = __GRADIENTS_60[ i * 3 + 2 ];

        QVector3D v( __GRADIENTS_60[ i * 3 ], __GRADIENTS_60[ i * 3+1 ], __GRADIENTS_60[ i * 3 + 2] );
        bvecs.push_back( v );
        bvals.push_back( 1000 );
    }


    for ( int i = 0; i < tessGradientsSize; ++i )
    {
        tessGradients( i + 1, 1 ) = tess::__TESS_THREE_V[ i * 3 ];
        tessGradients( i + 1, 2 ) = tess::__TESS_THREE_V[ i * 3 + 1 ];
        tessGradients( i + 1, 3 ) = tess::__TESS_THREE_V[ i * 3 + 2 ];
    }

    Matrix base( FMath::sh_base( gradients, max_ord ) );
    Matrix inv_base( FMath::pseudoInverse( FMath::sh_base( tessGradients, max_ord ) ) );

    for ( unsigned int i = 0; i < data->size(); ++i ) // for all voxels
    {
        for ( int k = 0; k < 3; ++k ) // for all 3 bingham peaks
        {
            ColumnVector sig( gradientsSize );
            sig = 0.0;

            f0 = data->at( i )[ k * 9 + 8 ];

            if ( f0 > 0.0 )
            {
                m1( 1 ) = data->at( i )[ k * 9 ];
                m1( 2 ) = data->at( i )[ k * 9 + 1 ];
                m1( 3 ) = data->at( i )[ k * 9 + 2 ];
                m2( 1 ) = data->at( i )[ k * 9 + 3 ];
                m2( 2 ) = data->at( i )[ k * 9 + 4 ];
                m2( 3 ) = data->at( i )[ k * 9 + 5 ];
                k1 = data->at( i )[ k * 9 + 6 ];
                k2 = data->at( i )[ k * 9 + 7 ];

                ColumnVector tmp( tessGradientsSize );
                tmp = 0.0;

                for ( int l = 0; l < tessGradientsSize; ++l )
                {
                    ColumnVector cur_dir(3);
                    cur_dir(1) = tess::__TESS_THREE_V[l*3];
                    cur_dir(2) = tess::__TESS_THREE_V[l*3+1];
                    cur_dir(3) = tess::__TESS_THREE_V[l*3+2];

                    tmp( l + 1 ) = f0 * exp( - ( k1 * FMath::iprod(cur_dir,m1) *
                                                      FMath::iprod(cur_dir,m1) +
                                                      k2 * FMath::iprod(cur_dir,m2) *
                                                      FMath::iprod(cur_dir,m2) ) );

                }
                // transform to spherical harmonic space
                ColumnVector sh_signal = inv_base * tmp;

                //do the convolution
                for( int order(0), j(0), m(0); order <= max_ord; order+=2, ++m )
                {
                    for( int degree( -order ); degree <= order; ++degree, ++j )
                    {
                        sh_signal(j+1) = sh_signal( j+1 ) * ( kernel[m] / boost::math::spherical_harmonic_r( order, 0, 0, 0 ) );
                    }
                }

                // back transform to signal space
                ColumnVector tmp1 = base * sh_signal;
                for ( int ii = 0; ii < tmp.Nrows(); ++ii )
                {
                    if ( tmp( ii + 1 ) > 1.0 )
                    {
                        tmp( ii + 1 ) = 1.0;
                    }
                }
                sigs[k]->at( i ) = tmp1;
            }

        }
    }

    std::vector<float> b0Data( data->size(), 220 );
    QList<Dataset*> dsout;
    for ( int i = 0; i < 3; ++i )
    {
        Writer writer( ds, QFileInfo() );
        DatasetDWI* out = new DatasetDWI( QDir( "dwifrombingham" ), *(sigs[i]), b0Data, bvals, bvecs, writer.createHeader( gradientsSize + 1 ) );

        dsout.push_back( out );
    }

    qDebug() << "finished bingham to dwi";

    return dsout;
}
