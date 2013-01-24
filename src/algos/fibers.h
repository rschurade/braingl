/*
 * fibers.h
 *
 *  Created on: Jan 24, 2013
 *      Author: schurade
 */

#ifndef FIBERS_H_
#define FIBERS_H_

#include <QtCore/QVector>

class DatasetFibers;

class Fibers
{
public:
    Fibers( DatasetFibers* ds );
    virtual ~Fibers();

    DatasetFibers* thinOut();

private:
    DatasetFibers* m_dataset;

    float getFiberDist( QVector< float >& lhs, QVector< float >& rhs );
    float getDist( float x1, float y1, float z1, float x2, float y2, float z2 );

    QVector<float> mergeFibs( QVector< float >& lhs, QVector< float >& rhs );

};

#endif /* FIBERS_H_ */
