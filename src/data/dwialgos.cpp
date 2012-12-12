/*
 * dwialgos.cpp
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */
#include <stdexcept>

#include <math.h>
#include "../algos/fmath.h"
#include "../algos/track.h"

#include <QtCore/QDebug>
#include <QtCore/QVector>
#include <QtGui/QVector3D>

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

#include "mesh/tesselation.h"

#include "datasets/dataset3d.h"
#include "datasets/datasetbingham.h"
#include "datasets/datasetdwi.h"
#include "datasets/datasetfibers.h"
#include "datasets/datasetscalar.h"
#include "datasets/datasettensor.h"
#include "datasets/datasetsh.h"

#include "../algos/qball.h"
#include "../algos/bingham.h"
#include "dwialgos.h"


DWIAlgos::DWIAlgos()
{
}

DWIAlgos::~DWIAlgos()
{
}

DatasetSH* DWIAlgos::qBall( DatasetDWI* ds )
{
    qDebug() << "start calculating qBall";
    QVector<QVector3D> bvecs = ds->getBvecs();

    Matrix gradients( bvecs.size(), 3 );
    for ( int i = 0; i < bvecs.size(); ++i )
    {
        gradients( i + 1, 1 ) = bvecs.at( i ).x();
        gradients( i + 1, 2 ) = bvecs.at( i ).y();
        gradients( i + 1, 3 ) = bvecs.at( i ).z();
    }

    double lambda = 0.006;
    int order = 4;
    Matrix qBallBase = QBall::calcQBallBase( gradients, lambda, order );

    QVector<ColumnVector>* data = ds->getData();

    QVector<ColumnVector>* qBallVector = new QVector<ColumnVector>();

    for ( int i = 0; i < data->size(); ++i )
    {
        qBallVector->push_back( qBallBase * data->at( i ) );
    }

    DatasetSH* out = new DatasetSH( "Q-Ball", qBallVector, ds->getHeader() );
    out->setProperty( "fileName", "QBall" );
    out->setProperty( "name", "QBall" );
    out->setProperty( "createdBy", FNALGO_QBALL );
    out->setProperty( "lod", 2 );
    out->setProperty( "order", order );
    out->setProperty( "renderSlice", 1 );
    out->setProperty( "scaling", 1.0 );
    out->setProperty( "nt", qBallVector->at( 0 ).Nrows() );
    out->setProperty( "datatype", DT_FLOAT );

    qDebug() << "finished calculating qBall";

    return out;
}

DatasetSH* DWIAlgos::qBallSharp( DatasetDWI* ds, int order )
{
    QVector<ColumnVector>* qBallVector = QBall::sharpQBall( ds, order );
    qDebug() << "create dataset";
    DatasetSH* out = new DatasetSH( "Q-Ball", qBallVector, ds->getHeader() );
    out->setProperty( "fileName", "Q-Ball" );

    QString name = QString( "Qball_" + QString::number( order ) + "_" + ds->getProperty( "name" ).toString() );
    out->setProperty( "name", name );
    out->setProperty( "createdBy", FNALGO_QBALL );
    out->setProperty( "lod", 2 );
    out->setProperty( "order", order );
    out->setProperty( "renderSlice", 1 );
    out->setProperty( "scaling", 1.0 );
    out->setProperty( "nt", qBallVector->at( 0 ).Nrows() );
    out->setProperty( "datatype", DT_FLOAT );

    qDebug() << "finished calculating qBall";

    return out;

}

DatasetTensor* DWIAlgos::tensorFit( DatasetDWI* ds )
{
    QVector<QVector3D> bvecs = ds->getBvecs();
    QVector<float> bvals = ds->getBvals();
    QVector<ColumnVector>* data = ds->getData();
    QVector<float> b0Images = ds->getB0Data();

    QVector<Matrix>* tensors = FMath::fitTensors( data, b0Images, bvecs, bvals );

    DatasetTensor* out = new DatasetTensor( ds->getProperty( "fileName" ).toString(), tensors, ds->getHeader() );
    out->setProperty( "fileName", "Tensor" );
    out->setProperty( "name", "Tensor" );
    out->setProperty( "createdBy", FNALGO_TENSORFIT );
    out->setProperty( "nt", 9 );
    out->setProperty( "datatype", DT_FLOAT );
    return out;
}

DatasetScalar* DWIAlgos::calcFAFromDWI( DatasetDWI* ds )
{
    QVector<QVector3D> bvecs = ds->getBvecs();
    QVector<float> bvals = ds->getBvals();
    QVector<ColumnVector>* data = ds->getData();
    QVector<float> b0Images = ds->getB0Data();

    QVector<Matrix>* tensors = FMath::fitTensors( data, b0Images, bvecs, bvals );

    QVector<float> fa;
    FMath::fa( tensors, fa );

    DatasetScalar* out = new DatasetScalar( "fa.nii.gz", fa, ds->getHeader() );
    out->setProperty( "fileName", "FA" );
    out->setProperty( "name", "FA" );
    out->setProperty( "createdBy", FNALGO_FA );
    out->setProperty( "nt", 1 );
    out->setProperty( "datatype", DT_FLOAT );
    return out;
}

QList<Dataset*> DWIAlgos::calcEVFromDWI( DatasetDWI* ds )
{
    QVector<QVector3D> bvecs = ds->getBvecs();
    QVector<float> bvals = ds->getBvals();
    QVector<ColumnVector>* data = ds->getData();
    QVector<float> b0Images = ds->getB0Data();

    QVector<Matrix>* tensors = FMath::fitTensors( data, b0Images, bvecs, bvals );

    int blockSize = tensors->size();

    QVector<QVector3D> evec1( blockSize );
    QVector<float> eval1( blockSize );

    QVector<QVector3D> evec2( blockSize );
    QVector<float> eval2( blockSize );

    QVector<QVector3D> evec3( blockSize );
    QVector<float> eval3( blockSize );

    FMath::evecs( tensors, evec1, eval2, evec1, eval2, evec3, eval3 );

    QList<Dataset*> l;

    Dataset3D* out = new Dataset3D( "evec1.nii.gz", evec1, ds->getHeader() );
    out->setProperty( "name", "evec 1" );
    out->setProperty( "createdBy", FNALGO_EV );
    out->setProperty( "nt", 3 );
    out->setProperty( "datatype", DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( "eval1.nii.gz", eval1, ds->getHeader() );
    out2->setProperty( "name", "eval 1" );
    out2->setProperty( "createdBy", FNALGO_EV );
    out2->setProperty( "nt", 1 );
    out2->setProperty( "datatype", DT_FLOAT );

    l.push_back( out );
    l.push_back( out2 );

    return l;
}

DatasetScalar* DWIAlgos::calcFAFromTensor( DatasetTensor* ds )
{
    QVector<Matrix>* tensors = ds->getData();

    QVector<float> fa;
    FMath::fa( tensors, fa );

    DatasetScalar* out = new DatasetScalar( "fa.nii.gz", fa, ds->getHeader() );
    out->setProperty( "fileName", "FA" );
    out->setProperty( "name", "FA" );
    out->setProperty( "createdBy", FNALGO_FA );
    out->setProperty( "nt", 1 );
    out->setProperty( "datatype", DT_FLOAT );
    return out;
}

QList<Dataset*> DWIAlgos::calcEVFromTensor( DatasetTensor* ds )
{
    QVector<Matrix>* tensors = ds->getData();

    int blockSize = tensors->size();

    QVector<QVector3D> evec1( blockSize );
    QVector<float> eval1( blockSize );

    QVector<QVector3D> evec2( blockSize );
    QVector<float> eval2( blockSize );

    QVector<QVector3D> evec3( blockSize );
    QVector<float> eval3( blockSize );

    FMath::evecs( tensors, evec1, eval2, evec1, eval2, evec3, eval3 );

    QList<Dataset*> l;

    Dataset3D* out = new Dataset3D( "evec1.nii.gz", evec1, ds->getHeader() );
    out->setProperty( "name", "evec 1" );
    out->setProperty( "createdBy", FNALGO_EV );
    out->setProperty( "nt", 3 );
    out->setProperty( "datatype", DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( "eval1.nii.gz", eval1, ds->getHeader() );
    out2->setProperty( "name", "eval 1" );
    out2->setProperty( "createdBy", FNALGO_EV );
    out2->setProperty( "nt", 1 );
    out2->setProperty( "datatype", DT_FLOAT );

    l.push_back( out );
    l.push_back( out2 );

    return l;
}

QList<Dataset*> DWIAlgos::fitBingham( DatasetSH* ds )
{
    int depth = 3;
    int neighbourhood = 2;
    int n_peaks = 3;

    QList<Dataset*> l= Bingham::calc_bingham( ds, depth, neighbourhood, n_peaks );
    if ( l.size() > 0 )
    {
        l[0]->setProperty( "name", "bingham" );
        l[0]->setProperty( "createdBy", FNALGO_BINGHAM );
        l[0]->setProperty( "nt", 42 );
        l[0]->setProperty( "datatype", DT_FLOAT );
    }
    if ( l.size() > 1 )
    {
        l[1]->setProperty( "name", "bingham tensor 1" );
        l[1]->setProperty( "createdBy", FNALGO_TENSORFIT );
        l[1]->setProperty( "nt", 6 );
        l[1]->setProperty( "datatype", DT_FLOAT );
        //l[1]->setProperty( "active", false );
    }
    if ( l.size() > 2 )
    {
        l[2]->setProperty( "name", "bingham tensor 2" );
        l[2]->setProperty( "createdBy", FNALGO_TENSORFIT );
        l[2]->setProperty( "nt", 6 );
        l[2]->setProperty( "datatype", DT_FLOAT );
        l[2]->setProperty( "active", false );
    }
    if ( l.size() > 3 )
    {
        l[3]->setProperty( "name", "bingham tensor 3" );
        l[3]->setProperty( "createdBy", FNALGO_TENSORFIT );
        l[3]->setProperty( "nt", 6 );
        l[3]->setProperty( "datatype", DT_FLOAT );
        l[3]->setProperty( "active", false );
    }

    return l;
}

QList<Dataset*> DWIAlgos::tensorTrack( DatasetTensor* ds )
{
    Track* tracker = new Track( ds );
    tracker->startTracking();

    QList<Dataset*> l;
    DatasetFibers* fibs = new DatasetFibers( tracker->getFibs(), tracker->getNumPoints(), tracker->getNumLines() );
    l.push_back( fibs );

    return l;
}
