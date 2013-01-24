/*
 * dwialgos.cpp
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */
#include "enums.h"
#include "../algos/fmath.h"
#include "../algos/track.h"

#include <QtCore/QDebug>
#include <QtCore/QVector>
#include <QtGui/QVector3D>

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

#include "../gui/widgets/datasetselectionwidget.h"

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
    out->properties()->set( FNPROP_FILENAME, "QBall" );
    out->properties()->set( FNPROP_NAME, "QBall" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_QBALL );
    out->properties()->set( FNPROP_LOD, 2 );
    out->properties()->set( FNPROP_ORDER, order );
    out->properties()->set( FNPROP_RENDER_SLICE, 1 );
    out->properties()->set( FNPROP_SCALING, 1.0f );
    out->properties()->set( FNPROP_DIM, qBallVector->at( 0 ).Nrows() );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    qDebug() << "finished calculating qBall";

    return out;
}

DatasetSH* DWIAlgos::qBallSharp( DatasetDWI* ds, int order )
{
    QVector<ColumnVector>* qBallVector = QBall::sharpQBall( ds, order );
    qDebug() << "create dataset";
    DatasetSH* out = new DatasetSH( "Q-Ball", qBallVector, ds->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "Q-Ball" );

    QString name = QString( "Qball_" + QString::number( order ) + "_" + ds->properties()->get( FNPROP_NAME ).toString() );
    out->properties()->set( FNPROP_NAME, name );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_QBALL );
    out->properties()->set( FNPROP_LOD, 2 );
    out->properties()->set( FNPROP_ORDER, order );
    out->properties()->set( FNPROP_RENDER_SLICE, 1 );
    out->properties()->set( FNPROP_SCALING, 1.0f );
    out->properties()->set( FNPROP_DIM, qBallVector->at( 0 ).Nrows() );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    qDebug() << "finished calculating qBall";

    return out;

}

DatasetTensor* DWIAlgos::tensorFit( DatasetDWI* ds )
{
    QVector<QVector3D> bvecs = ds->getBvecs();
    QVector<float> bvals = ds->getBvals();
    QVector<ColumnVector>* data = ds->getData();
    QVector<float>* b0Images = ds->getB0Data();

    QVector<Matrix>* tensors = FMath::fitTensors( data, b0Images, bvecs, bvals );

    DatasetTensor* out = new DatasetTensor( ds->properties()->get( FNPROP_FILENAME ).toString(), tensors, ds->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "Tensor" );
    out->properties()->set( FNPROP_NAME, "Tensor" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_TENSORFIT );
    out->properties()->set( FNPROP_DIM, 9 );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
    return out;
}

DatasetScalar* DWIAlgos::calcFAFromDWI( DatasetDWI* ds )
{
    QVector<QVector3D> bvecs = ds->getBvecs();
    QVector<float> bvals = ds->getBvals();
    QVector<ColumnVector>* data = ds->getData();
    QVector<float>* b0Images = ds->getB0Data();

    QVector<Matrix>* tensors = FMath::fitTensors( data, b0Images, bvecs, bvals );

    QVector<float> fa;
    FMath::fa( tensors, fa );

    DatasetScalar* out = new DatasetScalar( "fa.nii.gz", fa, ds->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "FA" );
    out->properties()->set( FNPROP_NAME, "FA" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_FA );
    out->properties()->set( FNPROP_DIM, 1 );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
    return out;
}

QList<Dataset*> DWIAlgos::calcEVFromDWI( DatasetDWI* ds )
{
    QVector<QVector3D> bvecs = ds->getBvecs();
    QVector<float> bvals = ds->getBvals();
    QVector<ColumnVector>* data = ds->getData();
    QVector<float>* b0Images = ds->getB0Data();

    QVector<Matrix>* tensors = FMath::fitTensors( data, b0Images, bvecs, bvals );

    int blockSize = tensors->size();

    QVector<QVector3D> evec1( blockSize );
    QVector<float> eval1( blockSize );

    QVector<QVector3D> evec2( blockSize );
    QVector<float> eval2( blockSize );

    QVector<QVector3D> evec3( blockSize );
    QVector<float> eval3( blockSize );

    FMath::evecs( tensors, evec1, eval1, evec2, eval2, evec3, eval3 );

    QList<Dataset*> l;

    Dataset3D* out = new Dataset3D( "evec1.nii.gz", evec1, ds->getHeader() );
    out->properties()->set( FNPROP_NAME, "evec 1" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out->properties()->set( FNPROP_DIM, 3 );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( "eval1.nii.gz", eval1, ds->getHeader() );
    out2->properties()->set( FNPROP_NAME, "eval 1" );
    out2->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out2->properties()->set( FNPROP_DIM, 1 );
    out2->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

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
    out->properties()->set( FNPROP_FILENAME, "FA" );
    out->properties()->set( FNPROP_NAME, "FA" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_FA );
    out->properties()->set( FNPROP_DIM, 1 );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
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
    out->properties()->set( FNPROP_NAME, "evec 1" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out->properties()->set( FNPROP_DIM, 3 );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( "eval1.nii.gz", eval1, ds->getHeader() );
    out2->properties()->set( FNPROP_NAME, "eval 1" );
    out2->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out2->properties()->set( FNPROP_DIM, 1 );
    out2->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    l.push_back( out );
    l.push_back( out2 );

    return l;
}

QList<Dataset*> DWIAlgos::fitBingham( DatasetSH* ds )
{
    int depth = 5;
    int neighbourhood = 3;
    int n_peaks = 3;

    QList<Dataset*> l= Bingham::calc_bingham( ds, depth, neighbourhood, n_peaks );
    if ( l.size() > 0 )
    {
        l[0]->properties()->set( FNPROP_NAME, "bingham" );
        l[0]->properties()->set( FNPROP_CREATED_BY, FNALGO_BINGHAM );
        l[0]->properties()->set( FNPROP_DIM, 42 );
        l[0]->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
        //l[0]->properties()->set( "active", false );
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

QList<Dataset*> DWIAlgos::bingham2Tensor( DatasetBingham* ds )
{
    QList<Dataset*> l= Bingham::bingham2Tensor( ds );
    for ( int i = 0; i < l.size(); ++i )
    {
        l[i]->properties()->set( FNPROP_FILENAME, "Tensor" );
        l[i]->properties()->set( FNPROP_NAME, "DWI FROM BINGHAM " + QString::number( i ) );
        l[i]->properties()->set( FNPROP_CREATED_BY, FNALGO_BINGHAM_2_TENSOR );
        l[i]->properties()->set( FNPROP_DIM, 9 );
        l[i]->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
    }
    return l;
}

QList<Dataset*> DWIAlgos::testAlgo( Dataset* ds, QList<Dataset*> &dsl )
{
    QVector< QPair<QString, FN_DATASET_TYPE> >filter;

    filter.push_back( QPair<QString, FN_DATASET_TYPE>( "a scalar dataset", FNDT_NIFTI_SCALAR ) );
    filter.push_back( QPair<QString, FN_DATASET_TYPE>( "another scalar dataset", FNDT_NIFTI_SCALAR ) );
    filter.push_back( QPair<QString, FN_DATASET_TYPE>( "a bingham dataset", FNDT_NIFTI_BINGHAM ) );
    filter.push_back( QPair<QString, FN_DATASET_TYPE>( "any nifti dataset", FNDT_NIFTI_ANY ) );

    DatasetSelectionWidget* dsw = new DatasetSelectionWidget( filter, dsl );
    dsw->show();
}
