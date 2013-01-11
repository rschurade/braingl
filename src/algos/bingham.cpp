/*
 * bingham.cpp
 *
 *  Created on: Nov 23, 2012
 *      Author: schurade
 */
#include <QtCore/QDebug>

#include "../data/mesh/tesselation.h"

#include "../data/datasets/dataset.h"
#include "../data/datasets/datasetsh.h"
#include "../data/datasets/datasetbingham.h"
#include "../data/datasets/datasettensor.h"

#include "fmath.h"
#include "sorts.h"

#include "binghamthread.h"
#include "bingham.h"

Bingham::Bingham()
{
}

Bingham::~Bingham()
{
}

QList<Dataset*> Bingham::calc_bingham( DatasetSH* sh, const int lod, const int neighbourhood, const int num_max )
{
    QList<Dataset*> dsout;
    QVector<ColumnVector>* data = sh->getData();

    const int order( ( -3 + static_cast<int>( sqrt( 8 * data->at( 0 ).Nrows() + 1 ) ) ) / 2 );
    qDebug() << "calculated order from sh: " << order;

    int numThreads = QThread::idealThreadCount();

    QVector<BinghamThread*> threads;
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

    QVector<QVector<float> >* out = new QVector<QVector<float> >();
    out->resize( data->size() );

    // combine fibs from all threads
    qDebug() << "combining results";
    QVector<float> v( 27, 0 );
    for ( int i = 0; i < data->size(); ++i )
    {
        out->replace( i, v );
    }


    for ( int i = 0; i < numThreads; ++i )
    {
        QVector<QVector<float> >* result = threads[i]->getResultVector();
        qDebug() << i << result->size();
        for ( int k = 0, l = i; k < result->size(); ++k, l += numThreads )
        {
            out->replace( l, result->at( k) );
        }
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }

    DatasetBingham* out1 = new DatasetBingham( "Bingham", out, sh->getHeader() );
    dsout.push_back( out1 );

    return dsout;
}
