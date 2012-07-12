/*
 * dwialgos.cpp
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */
#include <stdexcept>

#include <math.h>

#include <QtCore/QDebug>
#include <QtCore/QVector>
#include <QtGui/QVector3D>

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

#include "mesh/trianglemesh.h"
#include "mesh/tesselation.h"

#include "datasets/datasetdwi.h"
#include "datasets/datasetscalar.h"
#include "datasets/dataset3d.h"
#include "qball.h"
#include "dwialgos.h"


DWIAlgos::DWIAlgos()
{
}

DWIAlgos::~DWIAlgos()
{
}

DatasetDWI* DWIAlgos::qBall( DatasetDWI* ds )
{
    qDebug() << "start calculating qBall";
    QVector<QVector3D>bvecs =  ds->getBvecs();

    Matrix gradients( bvecs.size(), 3 );
    for ( int i = 0; i < bvecs.size(); ++i )
    {
        gradients( i+1, 1 ) = bvecs.at( i ).x();
        gradients( i+1, 2 ) = bvecs.at( i ).y();
        gradients( i+1, 3 ) = bvecs.at( i ).z();
    }

    double lambda = 0.006;
    int maxOrder = 6;
    Matrix qBallBase = QBall::calcQBallBase( gradients, lambda, maxOrder );

    qDebug() << "elements in data" << ds->getData()->at( 0 ).Nrows();
    qDebug() << "elements in qball base" << qBallBase.Nrows() << " " << qBallBase.Ncols();

    QVector<ColumnVector>* data = ds->getData();

    QVector<ColumnVector>* qballVector = new QVector<ColumnVector>();

    for ( int i = 0; i < data->size(); ++i )
    {
        qballVector->push_back( qBallBase * data->at( i ) );
    }

    DatasetDWI* out = new DatasetDWI( ds->getProperty( "fileName" ).toString(), qballVector, ds->getB0Data(), ds->getBvals(), bvecs );
    out->parseNiftiHeader( ds->getHeader() );
    out->setProperty( "fileName", "QBall" );
    out->setProperty( "name", "QBall" );
    out->setProperty( "createdBy", FNALGO_QBALL );
    out->setProperty( "lod", 2 );
    out->setProperty( "scaling", 1.0 );
    out->setProperty( "nt", qballVector->at(0).Nrows() );
    out->setProperty( "datatype", DT_FLOAT);
    out->examineDataset();

    qDebug() << "finished calculating qBall";

    return out;
}

DatasetDWI* DWIAlgos::tensorFit( DatasetDWI* ds )
{
    QVector<QVector3D>bvecs =  ds->getBvecs();
    QVector<int>bvals = ds->getBvals();

    int N = bvecs.size();

    Matrix B(N,6);
    Matrix U(N,6);
    Matrix V(6,6);
    Matrix BI(6,N);
    DiagonalMatrix D(6);

    double mult_c;

    for ( int i=0; i < N; ++i )
    {
        mult_c = (float)bvals[i] / (float)( bvecs[i].x()*bvecs[i].x() + bvecs[i].y()*bvecs[i].y() + bvecs[i].z()*bvecs[i].z() );

        B(i+1,1) =   mult_c * bvecs[i].x() * bvecs[i].x();
        B(i+1,2) = 2*mult_c * bvecs[i].x() * bvecs[i].y();
        B(i+1,3) = 2*mult_c * bvecs[i].x() * bvecs[i].z();
        B(i+1,4) =   mult_c * bvecs[i].y() * bvecs[i].y();
        B(i+1,5) = 2*mult_c * bvecs[i].y() * bvecs[i].z();
        B(i+1,6) =   mult_c * bvecs[i].z() * bvecs[i].z();
    }

    SVD(B, D, U, V);

    for( int j=1; j<=6;++j )
    {
        D(j)=1./D(j);
    }
    BI=V*D*U.t();

    double s0 = 0.0;
    double si = 0.0;
    vector<double>log_s0_si_pixel( N );

    QVector<ColumnVector>* data = ds->getData();
    QVector<ColumnVector>* tensors = new QVector<ColumnVector>();
    QVector<float> b0Images = ds->getB0Data();

    ColumnVector blank( 6 );
    blank = 0.0;

    for ( int i = 0; i < data->size(); ++i )
    {
        s0 = b0Images[i];

        if ( s0 > 0 )
        {
            // compute log(s0)-log(si)
            s0 = log(s0);
            for ( int j=0; j < N; ++j )
            {
                si = data->at(i)(j+1); //dti[j*blockSize+i];
                if (si > 0)
                {
                    si = log(si);
                }
                else
                {
                    si = 0.0;
                }
                log_s0_si_pixel[j] = s0 - si;
            }

            double value;
            // compute tensor
            ColumnVector t( 6 );
            for( int l=0; l<6; l++ )
            {
                value=0;
                for( size_t m = 1; m <= N; ++m )
                {
                    value += BI(l+1,m) * log_s0_si_pixel[m-1];
                }
                t( l+1 ) = (float) (value); // save the tensor components in a adjacent memory
            }
            tensors->push_back( t );
        } // end if s0 > 0
        else
        {
            tensors->push_back( blank );
        }
    }

    DatasetDWI* out = new DatasetDWI( ds->getProperty( "fileName" ).toString(), tensors, ds->getB0Data(), ds->getBvals(), bvecs );
    out->parseNiftiHeader( ds->getHeader() );
    out->setProperty( "fileName", "Tensor" );
    out->setProperty( "name", "Tensor" );
    out->setProperty( "createdBy", FNALGO_TENSORFIT );
    out->setProperty( "nt", 6 );
    out->setProperty( "datatype", DT_FLOAT);
    out->examineDataset();
    return out;
}

DatasetScalar* DWIAlgos::calcFA( DatasetDWI* ds )
{
    DatasetDWI* tensorDS = tensorFit( ds );
    QVector<ColumnVector>* tensors = tensorDS->getData();

    int blockSize = tensorDS->getData()->size();

    QVector<float> trace( blockSize );
    float value = 0;
    for ( size_t i = 0; i < blockSize; ++i )
    {
        value = tensors->at( i )( 1 );
        value += tensors->at( i )( 4 );
        value += tensors->at( i )( 6 );
        trace[i] = value/3.0;
    }

    QVector<float>fa( blockSize );

    double xx,xy,xz,yy,yz,zz,tr,AA,DD;

    for ( size_t i = 0; i < blockSize; ++i )
    {
        xx = tensors->at( i )(1);
        xy = tensors->at( i )(2);
        xz = tensors->at( i )(3);
        yy = tensors->at( i )(4);
        yz = tensors->at( i )(5);
        zz = tensors->at( i )(6);
        tr = trace[i];

        AA = pow2( xx - tr ) + pow2( yy - tr ) + pow2( zz - tr ) + 2 * pow2( xy ) + 2 * pow2( xz ) + 2 * pow2( yz );
        DD = pow2( xx ) + pow2( yy ) + pow2( zz ) + 2 * pow2( xy ) + 2 * pow2( xz ) + 2 * pow2( yz );

        if ( DD > 0 )
        {
            fa[ i ] = (float) ( sqrt( AA ) / sqrt( DD ) * sqrt( 1.5 ) );
        }
        else
        {
            fa[ i ] = 0.0;
        }
    }

    DatasetScalar* out = new DatasetScalar( "fa.nii.gz", fa );
    out->parseNiftiHeader( ds->getHeader() );
    out->setProperty( "fileName", "FA" );
    out->setProperty( "name", "FA" );
    out->setProperty( "createdBy", FNALGO_FA );
    out->setProperty( "nt", 1 );
    out->setProperty( "datatype", DT_FLOAT);
    out->examineDataset();

    return out;
}

QList<Dataset*> DWIAlgos::calcEV( DatasetDWI* ds )
{
    double xx,xy,xz,yy,yz,zz;
    double i1, i2, i3, v, s, phi, l1, l2, l3;
    double ev1_x, ev1_y, ev1_z, ev2_x, ev2_y, ev2_z, ev3_x, ev3_y, ev3_z, vec_norm;

    DatasetDWI* tensorDS = tensorFit( ds );
    QVector<ColumnVector>* tensors = tensorDS->getData();

    int blockSize = tensorDS->getData()->size();


    QVector<float> eval1( blockSize );
    QVector<float> eval2( blockSize );
    QVector<float> eval3( blockSize );
    QVector<QVector3D> evec1( blockSize );
    QVector<QVector3D> evec2( blockSize );
    QVector<QVector3D> evec3( blockSize );

    for ( size_t i = 0; i < blockSize; ++i )
    {
        xx = tensors->at( i )( 1 );
        xy = tensors->at( i )( 2 );
        xz = tensors->at( i )( 3 );
        yy = tensors->at( i )( 4 );
        yz = tensors->at( i )( 5 );
        zz = tensors->at( i )( 6 );

        // three invariants of D (dt)
        // i1=l1+l2+l3 (trace)
        // i2=l1*l2+l1*l3+l2*l3
        // i3=l1*l2*l3 (determinante)
        i1 = xx + yy + zz;
        i2 = xx*yy + xx*zz + yy*zz - ( pow2(xy) + pow2(xz) + pow2(yz) );
        i3 = xx*yy*zz+ 2.*xy*xz*yz - ( zz*pow2(xy) + yy*pow2(xz) + xx*pow2(yz));

        v = pow2(i1/3)-i2/3;
        s = pow3(i1/3)-i1*i2/6+i3/2;
        if ((v > 0) && (pow2(s) < pow3(v)))
          phi=acos(s/v*sqrt(1./v))/3;
        else phi = 0;

        // eigenvalues
        if (phi !=0) {
          l1=i1/3+2*sqrt(v)*cos(phi);
          l2=i1/3-2*sqrt(v)*cos(M_PI/3.+phi);
          l3=i1/3-2*sqrt(v)*cos(M_PI/3.-phi);
        }
        else
          l1=l2=l3=0.0;

        eval1[i] = l1;
        eval2[i] = l2;
        eval3[i] = l3;

        // eigenvectors
        ev1_x = (xy*yz-(yy-l1)*xz)* (xz*yz-(zz-l1)*xy);
        ev1_y = (xz*yz-(zz-l1)*xy)* (xz*xy-(xx-l1)*yz);
        ev1_z = (xy*yz-(yy-l1)*xz)* (xz*xy-(xx-l1)*yz);

        ev2_x = (xy*yz-(yy-l2)*xz)* (xz*yz-(zz-l2)*xy);
        ev2_y = (xz*yz-(zz-l2)*xy)* (xz*xy-(xx-l2)*yz);
        ev2_z = (xy*yz-(yy-l2)*xz)* (xz*xy-(xx-l2)*yz);

        ev3_x = (xy*yz-(yy-l3)*xz)* (xz*yz-(zz-l3)*xy);
        ev3_y = (xz*yz-(zz-l3)*xy)* (xz*xy-(xx-l3)*yz);
        ev3_z = (xy*yz-(yy-l3)*xz)* (xz*xy-(xx-l3)*yz);

        vec_norm = sqrt(pow2(ev1_x)+pow2(ev1_y)+pow2(ev1_z));

        if (vec_norm >0) {
          ev1_x = ev1_x/vec_norm;
          ev1_y = ev1_y/vec_norm;
          ev1_z = ev1_z/vec_norm;
        }
        else
          ev1_x=ev1_y=ev1_z=0.0;

        vec_norm=sqrt(pow2(ev2_x)+pow2(ev2_y)+pow2(ev2_z));

        if (vec_norm >0) {
          ev2_x = ev2_x/vec_norm;
          ev2_y = ev2_y/vec_norm;
          ev2_z = ev2_z/vec_norm;
        }
        else
          ev2_x=ev2_y=ev2_z=0.0;

        vec_norm=sqrt(pow2(ev3_x)+pow2(ev3_y)+pow2(ev3_z));

        if (vec_norm >0) {
          ev3_x = ev3_x/vec_norm;
          ev3_y = ev3_y/vec_norm;
          ev3_z = ev3_z/vec_norm;
        }
        else
          ev3_x=ev3_y=ev3_z=0.0;

        evec1[i].setX( ev1_x );
        evec1[i].setY( ev1_y );
        evec1[i].setZ( ev1_z );
        evec2[i].setX( ev2_x );
        evec2[i].setY( ev2_y );
        evec2[i].setZ( ev2_z );
        evec3[i].setX( ev3_x );
        evec3[i].setY( ev3_y );
        evec3[i].setZ( ev3_z );
    }

    QList<Dataset*>l;

    Dataset3D* out = new Dataset3D( "evec1.nii.gz", evec1 );
    out->parseNiftiHeader( ds->getHeader() );
    out->setProperty( "name", "evec 1" );
    out->setProperty( "createdBy", FNALGO_EV );
    out->setProperty( "nt", 3 );
    out->setProperty( "datatype", DT_FLOAT);
    out->examineDataset();

    DatasetScalar* out2 = new DatasetScalar( "eval1.nii.gz", eval1 );
    out2->parseNiftiHeader( ds->getHeader() );
    out2->setProperty( "name", "eval 1" );
    out2->setProperty( "createdBy", FNALGO_EV );
    out2->setProperty( "nt", 1 );
    out2->setProperty( "datatype", DT_FLOAT);
    out2->examineDataset();

    l.push_back( out );
    l.push_back( out2 );

    return l;
}
