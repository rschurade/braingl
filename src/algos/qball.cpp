/*
 * qball.cpp
 *
 *  Created on: 10.07.2012
 *      Author: Ralph
 */
#include "qball.h"
#include "sharpqballthread.h"

#include "fmath.h"

#include "../data/datasets/datasetdwi.h"

#include <QtCore/QDebug>
#include <QtCore/qmath.h>

#include <boost/math/special_functions/spherical_harmonic.hpp>

QBall::QBall()
{
}

QBall::~QBall()
{
}

Matrix QBall::calcQBallBase( Matrix gradients, double lambda, int maxOrder )
{
    //qDebug() << "start calculating qBall base";
    double sh_size( ( maxOrder + 1 ) * ( maxOrder + 2 ) / 2 );

    // check validity of input:
    if ( gradients.Nrows() == 0 )
        throw std::invalid_argument( "No gradient directions specified." );

    if ( gradients.Ncols() != 3 )
        throw std::invalid_argument( "Gradients have to be 3D." );

    // calculate spherical harmonics base:
    Matrix SH = FMath::sh_base( gradients, maxOrder );

    // calculate the Laplace-Beltrami and the Funk-Radon transformation:
    ColumnVector LBT( sh_size );
    ColumnVector FRT( sh_size );
    LBT = 0;
    FRT = 0;

    for ( int order = 0; order <= maxOrder; order += 2 )
    {
        double frt_val = 2.0 * M_PI * boost::math::legendre_p<double>( order, 0 );
        double lbt_val = lambda * order * order * ( order + 1 ) * ( order + 1 );

        for ( int degree( -order ); degree <= order; ++degree )
        {
            int i = order * ( order + 1 ) / 2 + degree;
            LBT( i + 1 ) = lbt_val;
            FRT( i + 1 ) = frt_val;
        }
    }

    // prepare the calculation of the pseudoinverse:
    Matrix B = SH.t() * SH;

    // update with Laplace-Beltrami operator:
    for ( int i = 0; i < sh_size; ++i )
    {
        B( i + 1, i + 1 ) += LBT( i + 1 );
    }

    Matrix out = B.i() * SH.t();

    // the Funk-Radon transformation:
    for ( int i = 0; i < B.Nrows(); ++i )
    {
        for ( int j = 0; j < out.Ncols(); ++j )
        {
            out( i + 1, j + 1 ) *= FRT( i + 1 );
        }
    }

    //qDebug() << "finished calculating qBall base";

    return out;
}

/*
 * CMRImage calc_sharp_q_ball(

 const CMRImage& data,
 const CMRImage& b_zero,
 const matrixT gradients,
 const baseT order,
 const rangeS r )
 {
 *
 */

void QBall::sharpQBall( DatasetDWI* ds, int order, QVector<ColumnVector>& out )
{
    int numThreads = QThread::idealThreadCount();

    QVector<SharpQBallThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new SharpQBallThread( ds, order, i ) );
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

    out.clear();
    // combine fibs from all threads
    for ( int i = 0; i < numThreads; ++i )
    {
        out += threads[i]->getQBallVector();
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }
}
