/*
 * dwialgos.cpp
 *
 * Created on: Jun 18, 2012
 * @author Ralph Schurade
 */
#include "dwialgos.h"

#include "fmath.h"
#include "track.h"
#include "qball.h"
#include "bingham.h"

#include "../data/enums.h"
#include "../data/mesh/trianglemesh2.h"
#include "../data/mesh/tesselation.h"
#include "../data/datasets/dataset3d.h"
#include "../data/datasets/datasetbingham.h"
#include "../data/datasets/datasetdwi.h"
#include "../data/datasets/datasetfibers.h"
#include "../data/datasets/datasetmesh.h"
#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasettensor.h"
#include "../data/datasets/datasetsh.h"

#include "../io/writer.h"

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

#include <QDebug>
#include <QVector>
#include <QVector3D>
#include <QString>

DWIAlgos::DWIAlgos()
{
}

DWIAlgos::~DWIAlgos()
{
}

QList<Dataset*> DWIAlgos::qBall( Dataset* ds )
{
    qDebug() << "start calculating qBall";
    std::vector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();

    Matrix gradients( bvecs.size(), 3 );
    for ( unsigned int i = 0; i < bvecs.size(); ++i )
    {
        gradients( i + 1, 1 ) = bvecs.at( i ).x();
        gradients( i + 1, 2 ) = bvecs.at( i ).y();
        gradients( i + 1, 3 ) = bvecs.at( i ).z();
    }

    double lambda = 0.006;
    int order = 4;
    Matrix qBallBase = QBall::calcQBallBase( gradients, lambda, order );

    std::vector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();

    std::vector<ColumnVector> qBallVector;

    for ( unsigned int i = 0; i < data->size(); ++i )
    {
        qBallVector.push_back( qBallBase * data->at( i ) );
    }

    Writer writer( ds, QFileInfo() );
    DatasetSH* out = new DatasetSH( QDir( "Q-Ball" ), qBallVector, writer.createHeader( data->at( 0 ).Nrows() ) );
    out->properties( "maingl" ).set( Fn::Property::D_NAME, "QBall" );
    out->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::QBALL );
    out->properties( "maingl" ).set( Fn::Property::D_LOD, 2 );
    out->properties( "maingl" ).set( Fn::Property::D_ORDER, order );
    out->properties( "maingl" ).set( Fn::Property::D_RENDER_SLICE, 1 );
    out->properties( "maingl" ).set( Fn::Property::D_SCALING, 1.0f );
    out->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
    out->properties( "maingl" ).set( Fn::Property::D_MINMAX_SCALING, true );

    qDebug() << "finished calculating qBall";

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::qBallSharp( Dataset* ds, int order )
{
    qDebug() << "start calculating qBall sharp";
    std::vector<ColumnVector> qBallVector;
    QBall::sharpQBall( dynamic_cast<DatasetDWI*>( ds ), order, qBallVector );
    qDebug() << "create dataset";

    QString name = QString( "Qball_" + QString::number( order ) + "_" + ds->properties( "maingl" ).get( Fn::Property::D_NAME ).toString() );

    Writer writer( ds, QFileInfo() );
    DatasetSH* out = new DatasetSH( QDir( name ), qBallVector, writer.createHeader( qBallVector.at( 0 ).Nrows() ) );
    out->properties( "maingl" ).set( Fn::Property::D_NAME, name );
    out->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::QBALL );
    out->properties( "maingl" ).set( Fn::Property::D_LOD, 2 );
    out->properties( "maingl" ).set( Fn::Property::D_ORDER, order );
    out->properties( "maingl" ).set( Fn::Property::D_RENDER_SLICE, 1 );
    out->properties( "maingl" ).set( Fn::Property::D_SCALING, 1.0f );
    out->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
    out->properties( "maingl" ).set( Fn::Property::D_MINMAX_SCALING, true );

    qDebug() << "finished calculating qBall";

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::tensorFit( Dataset* ds )
{
    std::vector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();
    std::vector<float> bvals = dynamic_cast<DatasetDWI*>( ds )->getBvals();
    std::vector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();
    std::vector<float>* b0Images = dynamic_cast<DatasetDWI*>( ds )->getB0Data();

    std::vector<Matrix> tensors;
    FMath::fitTensors( *data, *b0Images, bvecs, bvals, tensors );

    Writer writer( ds, QFileInfo() );
    DatasetTensor* out = new DatasetTensor( QDir( ds->properties( "maingl" ).get( Fn::Property::D_FILENAME ).toString() ), tensors, writer.createHeader( 6 ) );
    out->properties( "maingl" ).set( Fn::Property::D_NAME, "Tensor" );
    out->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::TENSORFIT );
    out->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::calcFAFromDWI( Dataset* ds )
{
    std::vector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();
    std::vector<float> bvals = dynamic_cast<DatasetDWI*>( ds )->getBvals();
    std::vector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();
    std::vector<float>* b0Images = dynamic_cast<DatasetDWI*>( ds )->getB0Data();

    std::vector<Matrix> tensors;
    FMath::fitTensors( *data, *b0Images, bvecs, bvals, tensors );

    std::vector<float> fa;
    FMath::fa( tensors, fa );

    Writer writer( ds, QFileInfo() );
    DatasetScalar* out = new DatasetScalar( QDir( "fa.nii.gz" ), fa, writer.createHeader( 1 ) );
    out->properties( "maingl" ).set( Fn::Property::D_NAME, "FA" );
    out->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::FA );
    out->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::calcEVFromDWI( Dataset* ds )
{
    std::vector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( ds )->getBvecs();
    std::vector<float> bvals = dynamic_cast<DatasetDWI*>( ds )->getBvals();
    std::vector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( ds )->getData();
    std::vector<float>* b0Images = dynamic_cast<DatasetDWI*>( ds )->getB0Data();

    std::vector<Matrix> tensors;
    FMath::fitTensors( *data, *b0Images, bvecs, bvals, tensors );

    int blockSize = tensors.size();

    std::vector<QVector3D> evec1( blockSize );
    std::vector<float> eval1( blockSize );

    std::vector<QVector3D> evec2( blockSize );
    std::vector<float> eval2( blockSize );

    std::vector<QVector3D> evec3( blockSize );
    std::vector<float> eval3( blockSize );

    FMath::evecs( tensors, evec1, eval1, evec2, eval2, evec3, eval3 );

    std::vector<float> fa;
    FMath::fa( tensors, fa );

    for ( unsigned int i = 0; i < evec1.size(); ++i )
    {
        evec2[i] = evec1[i] * fa[i] * 1.5;
    }

    QList<Dataset*> l;

    Writer writer( ds, QFileInfo() );
    Dataset3D* out = new Dataset3D( QDir( "evec1.nii.gz" ), evec1, writer.createHeader( 3 ) );
    out->properties( "maingl" ).set( Fn::Property::D_NAME, "evec 1" );
    out->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::EV );
    out->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
    l.push_back( out );

    Dataset3D* out2 = new Dataset3D( QDir( "fa_rgb.nii.gz" ), evec2, writer.createHeader( 3 ) );
    out2->properties( "maingl" ).set( Fn::Property::D_NAME, "fa rgb" );
    out2->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::EV );
    out2->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
    l.push_back( out2 );

    return l;
}

QList<Dataset*> DWIAlgos::calcFAFromTensor( Dataset* ds )
{
    std::vector<Matrix>* tensors = dynamic_cast<DatasetTensor*>( ds )->getData();

    std::vector<float> fa;
    FMath::fa( *tensors, fa );

    Writer writer( ds, QFileInfo() );
    DatasetScalar* out = new DatasetScalar( QDir( "fa.nii.gz" ), fa, writer.createHeader( 1 ) );
    out->properties( "maingl" ).set( Fn::Property::D_NAME, "FA" );
    out->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::FA );
    out->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );

    QList<Dataset*> l;
    l.push_back( out );
    return l;
}

QList<Dataset*> DWIAlgos::calcEVFromTensor( Dataset* ds )
{
    std::vector<Matrix>* tensors = dynamic_cast<DatasetTensor*>( ds )->getData();

    int blockSize = tensors->size();

    std::vector<QVector3D> evec1( blockSize );
    std::vector<float> eval1( blockSize );

    std::vector<QVector3D> evec2( blockSize );
    std::vector<float> eval2( blockSize );

    std::vector<QVector3D> evec3( blockSize );
    std::vector<float> eval3( blockSize );

    FMath::evecs( *tensors, evec1, eval1, evec2, eval2, evec3, eval3 );

    std::vector<float> fa;
    FMath::fa( *tensors, fa );

    for ( unsigned int i = 0; i < evec1.size(); ++i )
    {
        evec2[i] = evec1[i] * fa[i] * 1.5f;
    }

    QList<Dataset*> l;

    Writer writer( ds, QFileInfo() );
    Dataset3D* out = new Dataset3D( QDir( "evec1.nii.gz" ), evec1, writer.createHeader( 3 ) );
    out->properties( "maingl" ).set( Fn::Property::D_NAME, "evec 1" );
    out->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::EV );
    out->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
    l.push_back( out );

    Dataset3D* out2 = new Dataset3D( QDir( "fa_rgb.nii.gz" ), evec2, writer.createHeader( 3 ) );
    out2->properties( "maingl" ).set( Fn::Property::D_NAME, "fa rgb" );
    out2->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::EV );
    out2->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
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
        l[0]->properties( "maingl" ).set( Fn::Property::D_NAME, "bingham" );
        l[0]->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::BINGHAM );
        l[0]->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
        //l[0]->properties( "maingl" )->set( "active", false );
    }
    return l;
}

QList<Dataset*> DWIAlgos::tensorTrack( Dataset* ds )
{
    Track* tracker = new Track( dynamic_cast<DatasetTensor*>( ds ) );
    tracker->startTracking();
    QList<Dataset*> l;
    if ( tracker->getFibs().size() > 0 )
    {
        QList<QString>dataNames;
        dataNames.push_back( "fa" );
        DatasetFibers* fibs = new DatasetFibers( QDir( "new fibers"  ), tracker->getFibs(), dataNames );
        l.push_back( fibs );
    }
    return l;
}

QList<Dataset*> DWIAlgos::bingham2DWI( Dataset* ds )
{
    QList<Dataset*> l= Bingham::bingham2Tensor( dynamic_cast<DatasetBingham*>( ds ) );
    for ( int i = 0; i < l.size(); ++i )
    {
        l[i]->properties( "maingl" ).set( Fn::Property::D_NAME, "DWI FROM BINGHAM " + QString::number( i ) );
        l[i]->properties( "maingl" ).set( Fn::Property::D_CREATED_BY, (int)Fn::Algo::BINGHAM_2_TENSOR );
        l[i]->properties( "maingl" ).set( Fn::Property::D_DATATYPE, DT_FLOAT );
    }
    return l;
}

QList<Dataset*> DWIAlgos::sh2mesh( Dataset* ds )
{
    DatasetSH* dsh = dynamic_cast<DatasetSH*>( ds );

    QList<Dataset*> l;
    DatasetMesh* dsm = new DatasetMesh( dsh->getMeshFromCurrent(), QDir( "SH Mesh" ) );
    l.push_back( dsm );
    return l;
}

