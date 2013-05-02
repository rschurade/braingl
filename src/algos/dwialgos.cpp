/*
 * dwialgos.cpp
 *
 * Created on: Jun 18, 2012
 * @author Ralph Schurade
 */
#include "dwialgos.h"
#include "../data/enums.h"

#include "../data/mesh/tesselation.h"
#include "../data/datasets/dataset3d.h"
#include "../data/datasets/datasetbingham.h"
#include "../data/datasets/datasetdwi.h"
#include "../data/datasets/datasetfibers.h"
#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasettensor.h"
#include "../data/datasets/datasetsh.h"

#include "fmath.h"
#include "track.h"
#include "qball.h"
#include "bingham.h"

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

#include <QDebug>
#include <QVector>
#include <QVector3D>


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

    DatasetSH* out = new DatasetSH( QDir( "Q-Ball" ), qBallVector, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties( "maingl" )->set( Fn::Property::NAME, "QBall" );
    out->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::QBALL );
    out->properties( "maingl" )->set( Fn::Property::LOD, 2 );
    out->properties( "maingl" )->set( Fn::Property::ORDER, order );
    out->properties( "maingl" )->set( Fn::Property::RENDER_SLICE, 1 );
    out->properties( "maingl" )->set( Fn::Property::SCALING, 1.0f );
    out->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );
    out->properties( "maingl" )->set( Fn::Property::MINMAX_SCALING, true );

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

    QString name = QString( "Qball_" + QString::number( order ) + "_" + ds->properties( "maingl" )->get( Fn::Property::NAME ).toString() );

    DatasetSH* out = new DatasetSH( QDir( name ), qBallVector, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties( "maingl" )->set( Fn::Property::NAME, name );
    out->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::QBALL );
    out->properties( "maingl" )->set( Fn::Property::LOD, 2 );
    out->properties( "maingl" )->set( Fn::Property::ORDER, order );
    out->properties( "maingl" )->set( Fn::Property::RENDER_SLICE, 1 );
    out->properties( "maingl" )->set( Fn::Property::SCALING, 1.0f );
    out->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );
    out->properties( "maingl" )->set( Fn::Property::MINMAX_SCALING, true );

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

    DatasetTensor* out = new DatasetTensor( QDir( ds->properties( "maingl" )->get( Fn::Property::FILENAME ).toString() ), tensors, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties( "maingl" )->set( Fn::Property::NAME, "Tensor" );
    out->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::TENSORFIT );
    out->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );

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

    DatasetScalar* out = new DatasetScalar( QDir( "fa.nii.gz" ), fa, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties( "maingl" )->set( Fn::Property::NAME, "FA" );
    out->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::FA );
    out->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );

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

    Dataset3D* out = new Dataset3D( QDir( "evec1.nii.gz" ), evec1, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out->properties( "maingl" )->set( Fn::Property::NAME, "evec 1" );
    out->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::EV );
    out->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( QDir( "eval1.nii.gz" ), eval1, dynamic_cast<DatasetDWI*>( ds )->getHeader() );
    out2->properties( "maingl" )->set( Fn::Property::NAME, "eval 1" );
    out2->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::EV );
    out2->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );

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

    DatasetScalar* out = new DatasetScalar( QDir( "fa.nii.gz" ), fa, dynamic_cast<DatasetTensor*>( ds )->getHeader() );
    out->properties( "maingl" )->set( Fn::Property::NAME, "FA" );
    out->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::FA );
    out->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );

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

    Dataset3D* out = new Dataset3D( QDir( "evec1.nii.gz" ), evec1, dynamic_cast<DatasetTensor*>( ds )->getHeader() );
    out->properties( "maingl" )->set( Fn::Property::NAME, "evec 1" );
    out->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::EV );
    out->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );

    DatasetScalar* out2 = new DatasetScalar( QDir( "eval1.nii.gz" ), eval1, dynamic_cast<DatasetTensor*>( ds )->getHeader() );
    out2->properties( "maingl" )->set( Fn::Property::NAME, "eval 1" );
    out2->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::EV );
    out2->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );

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
        l[0]->properties( "maingl" )->set( Fn::Property::NAME, "bingham" );
        l[0]->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::BINGHAM );
        l[0]->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );
        //l[0]->properties( "maingl" )->set( "active", false );
    }
    return l;
}

QList<Dataset*> DWIAlgos::tensorTrack( Dataset* ds )
{
    Track* tracker = new Track( dynamic_cast<DatasetTensor*>( ds ) );
    tracker->startTracking();

    QList<Dataset*> l;
    DatasetFibers* fibs = new DatasetFibers( QDir( "new fibers"  ), tracker->getFibs() );
    l.push_back( fibs );

    return l;
}

QList<Dataset*> DWIAlgos::bingham2DWI( Dataset* ds )
{
    QList<Dataset*> l= Bingham::bingham2Tensor( dynamic_cast<DatasetBingham*>( ds ) );
    for ( int i = 0; i < l.size(); ++i )
    {
        l[i]->properties( "maingl" )->set( Fn::Property::NAME, "DWI FROM BINGHAM " + QString::number( i ) );
        l[i]->properties( "maingl" )->set( Fn::Property::CREATED_BY, (int)Fn::Algo::BINGHAM_2_TENSOR );
        l[i]->properties( "maingl" )->set( Fn::Property::DATATYPE, DT_FLOAT );
    }
    return l;
}
