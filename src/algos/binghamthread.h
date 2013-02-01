/*
 * bingham.h
 *
 *  Created on: 27.12.2012
 *      Author: Ralph
 */

#ifndef BINGHAMTHREAD_H_
#define BINGHAMTHREAD_H_

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QVector>

class DatasetSH;

class BinghamThread : public QThread
{
public:
    BinghamThread( DatasetSH* ds, int lod, int id );
    virtual ~BinghamThread();

    QVector<QVector<float> > getResultVector();

private:
    void run();

    QVector<float> fit_bingham( const ColumnVector& sh_data,
                                  const Matrix& tess,
                                  const QVector<QSet<int> >& adj,
                                  const Matrix& base,
                                  const int neighborhood,
                                  const int num_max );


    DatasetSH* m_ds;
    int m_lod;

    int m_id;

    QVector<QVector<float> > m_resultVector;
};

#endif /* BINGHAMTHREAD_H_ */
