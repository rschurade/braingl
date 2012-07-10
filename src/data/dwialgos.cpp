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
    int maxOrder = 4;
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


