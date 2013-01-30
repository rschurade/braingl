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

#include "../gui/widgets/algoStarterWidgets/datasetselectionwidget.h"

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

QList<Dataset*> DWIAlgos::qBall( Dataset* ds )
{
    qDebug() << "start calculating qBall";
    QVector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();

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

    QVector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();

    QVector<ColumnVector> qBallVector;

    for ( int i = 0; i < data->size(); ++i )
    {
        qBallVector.push_back( qBallBase * data->at( i ) );
    }

    DatasetSH* out = new DatasetSH( "Q-Ball", qBallVector, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "QBall" );
    out->properties()->set( FNPROP_NAME, "QBall" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_QBALL );
    out->properties()->set( FNPROP_LOD, 2 );
    out->properties()->set( FNPROP_ORDER, order );
    out->properties()->set( FNPROP_RENDER_SLICE, 1 );
    out->properties()->set( FNPROP_SCALING, 1.0f );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
    out->properties()->set( FNPROP_MINMAX_SCALING, true );

    qDebug() << "finished calculating qBall";

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::qBallSharp( Dataset* ds, int order )
{
    QVector<ColumnVector> qBallVector;
    QBall::sharpQBall( dynamic_cast<DatasetDWI*>( ds ), order, qBallVector );
    qDebug() << "create dataset";
    DatasetSH* out = new DatasetSH( "Q-Ball", qBallVector, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "Q-Ball" );

    QString name = QString( "Qball_" + QString::number( order ) + "_" + ds->properties()->get( FNPROP_NAME ).toString() );
    out->properties()->set( FNPROP_NAME, name );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_QBALL );
    out->properties()->set( FNPROP_LOD, 2 );
    out->properties()->set( FNPROP_ORDER, order );
    out->properties()->set( FNPROP_RENDER_SLICE, 1 );
    out->properties()->set( FNPROP_SCALING, 1.0f );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
    out->properties()->set( FNPROP_MINMAX_SCALING, true );

    qDebug() << "finished calculating qBall";

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::tensorFit( Dataset* ds )
{
    QVector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();
    QVector<float> bvals = dynamic_cast<DatasetDWI*>( ds )->getBvals();
    QVector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();
    QVector<float>* b0Images = dynamic_cast<DatasetDWI*>( ds )->getB0Data();

    QVector<Matrix> tensors;
    FMath::fitTensors( *data, *b0Images, bvecs, bvals, tensors );

    DatasetTensor* out = new DatasetTensor( ds->properties()->get( FNPROP_FILENAME ).toString(), tensors, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "Tensor" );
    out->properties()->set( FNPROP_NAME, "Tensor" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_TENSORFIT );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::calcFAFromDWI( Dataset* ds )
{
    QVector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();
    QVector<float> bvals = dynamic_cast<DatasetDWI*>( ds )->getBvals();
    QVector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();
    QVector<float>* b0Images = dynamic_cast<DatasetDWI*>( ds )->getB0Data();

    QVector<Matrix> tensors;
    FMath::fitTensors( *data, *b0Images, bvecs, bvals, tensors );

    QVector<float> fa;
    FMath::fa( tensors, fa );

    DatasetScalar* out = new DatasetScalar( "fa.nii.gz", fa, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "FA" );
    out->properties()->set( FNPROP_NAME, "FA" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_FA );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::calcEVFromDWI( Dataset* ds )
{
    QVector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();
    QVector<float> bvals = dynamic_cast<DatasetDWI*>( ds )->getBvals();
    QVector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();
    QVector<float>* b0Images = dynamic_cast<DatasetDWI*>( ds )->getB0Data();

    QVector<Matrix> tensors;
    FMath::fitTensors( *data, *b0Images, bvecs, bvals, tensors );

    int blockSize = tensors.size();

    QVector<QVector3D> evec1( blockSize );
    QVector<float> eval1( blockSize );

    QVector<QVector3D> evec2( blockSize );
    QVector<float> eval2( blockSize );

    QVector<QVector3D> evec3( blockSize );
    QVector<float> eval3( blockSize );

    FMath::evecs( tensors, evec1, eval1, evec2, eval2, evec3, eval3 );

    Dataset3D* out = new Dataset3D( "evec1.nii.gz", evec1, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties()->set( FNPROP_NAME, "evec 1" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( "eval1.nii.gz", eval1, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out2->properties()->set( FNPROP_NAME, "eval 1" );
    out2->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out2->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    l.push_back( out2 );
    return l;
}

QList<Dataset*> DWIAlgos::calcFAFromTensor( Dataset* ds )
{
    QVector<Matrix>* tensors = dynamic_cast<DatasetTensor*>( ds )->getData();

    QVector<float> fa;
    FMath::fa( *tensors, fa );

    DatasetScalar* out = new DatasetScalar( "fa.nii.gz", fa, dynamic_cast<DatasetTensor*>( ds )->getHeader() );
    out->properties()->set( FNPROP_FILENAME, "FA" );
    out->properties()->set( FNPROP_NAME, "FA" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_FA );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::calcEVFromTensor( Dataset* ds )
{
    QVector<Matrix>* tensors = dynamic_cast<DatasetTensor*>( ds )->getData();

    int blockSize = tensors->size();

    QVector<QVector3D> evec1( blockSize );
    QVector<float> eval1( blockSize );

    QVector<QVector3D> evec2( blockSize );
    QVector<float> eval2( blockSize );

    QVector<QVector3D> evec3( blockSize );
    QVector<float> eval3( blockSize );

    FMath::evecs( *tensors, evec1, eval1, evec2, eval2, evec3, eval3 );

    Dataset3D* out = new Dataset3D( "evec1.nii.gz", evec1, dynamic_cast<DatasetTensor*>( ds )->getHeader() );
    out->properties()->set( FNPROP_NAME, "evec 1" );
    out->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( "eval1.nii.gz", eval1, dynamic_cast<DatasetTensor*>( ds )->getHeader() );
    out2->properties()->set( FNPROP_NAME, "eval 1" );
    out2->properties()->set( FNPROP_CREATED_BY, FNALGO_EV );
    out2->properties()->set( FNPROP_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    l.push_back( out2 );
    return l;
}

QList<Dataset*> DWIAlgos::fitBingham( Dataset* ds )
{
    int depth = 5;
    int neighbourhood = 3;
    int n_peaks = 3;

    QList<Dataset*> l= Bingham::calc_bingham( dynamic_cast<DatasetSH*>( ds ), depth, neighbourhood, n_peaks );
    if ( l.size() > 0 )
    {
        l[0]->properties()->set( FNPROP_NAME, "bingham" );
        l[0]->properties()->set( FNPROP_CREATED_BY, FNALGO_BINGHAM );
        l[0]->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
        //l[0]->properties()->set( "active", false );
    }
    return l;
}

QList<Dataset*> DWIAlgos::tensorTrack( Dataset* ds )
{
    Track* tracker = new Track( dynamic_cast<DatasetTensor*>( ds ) );
    tracker->startTracking();

    QList<Dataset*> l;
    DatasetFibers* fibs = new DatasetFibers( tracker->getFibs(), tracker->getNumPoints(), tracker->getNumLines() );
    l.push_back( fibs );

    return l;
}

QList<Dataset*> DWIAlgos::bingham2DWI( Dataset* ds )
{
    QList<Dataset*> l= Bingham::bingham2Tensor( dynamic_cast<DatasetBingham*>( ds ) );
    for ( int i = 0; i < l.size(); ++i )
    {
        l[i]->properties()->set( FNPROP_FILENAME, "Tensor" );
        l[i]->properties()->set( FNPROP_NAME, "DWI FROM BINGHAM " + QString::number( i ) );
        l[i]->properties()->set( FNPROP_CREATED_BY, FNALGO_BINGHAM_2_TENSOR );
        l[i]->properties()->set( FNPROP_DATATYPE, DT_FLOAT );
    }
    return l;
}

QList<Dataset*> DWIAlgos::testAlgo( Dataset* ds, QList<Dataset*> &dsl )
{
    QVector< QPair<QString, FN_DATASET_TYPE> >filter;
    filter.push_back( QPair<QString, FN_DATASET_TYPE>( "optional: mask", FNDT_NIFTI_SCALAR ) );

    DatasetSelectionWidget* dsw = new DatasetSelectionWidget( filter, dsl );
    dsw->show();
    QList<Dataset*>l;
    return l;
}
