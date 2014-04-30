/*
 * bingham.h
 *
 * Created on: 27.12.2012
 * @author Ralph Schurade
 */

#ifndef BINGHAMTHREAD_H_
#define BINGHAMTHREAD_H_

#include "../thirdparty/newmat10/newmat.h"
#include "../thirdparty/newmat10/newmatap.h"

#include <QDebug>
#include <QThread>
#include <QVector>

class DatasetSH;

class BinghamThread : public QThread
{
public:
    BinghamThread( DatasetSH* ds, int lod, int id );
    virtual ~BinghamThread();

    std::vector<std::vector<float> > getResultVector();

private:
    void run();

    std::vector<float> fit_bingham( const ColumnVector& sh_data,
                                  const Matrix& tess,
                                  const std::vector<QSet<int> >& adj,
                                  const Matrix& base,
                                  const int neighborhood,
                                  const int num_max );


    DatasetSH* m_ds;
    int m_lod;

    int m_id;

    std::vector<std::vector<float> > m_resultVector;
};

#endif /* BINGHAMTHREAD_H_ */
