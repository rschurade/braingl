/*
 * fibers.h
 *
 * Created on: Jan 24, 2013
 * @author Ralph Schurade
 */

#ifndef FIBERS_H_
#define FIBERS_H_

#include "fib.h"

#include <QVector>

class DatasetFibers;
class DatasetScalar;
class Dataset3D;

class Fibers
{
public:
    Fibers( DatasetFibers* ds );
    virtual ~Fibers();

    DatasetFibers* thinOut();
    DatasetScalar* tractDensity();
    Dataset3D* tractColor();
    DatasetFibers* downSample();

private:
    DatasetFibers* m_dataset;

    Fib mergeFibs( Fib& lhs, Fib& rhs );

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;
    int m_blockSize;

    int getID( float x, float y, float z )
    {
        int id = (int) ( x / m_dx ) + (int) ( y / m_dy ) * m_nx + (int) ( z / m_dz ) * m_ny * m_nx;

        id = std::max( (int) 0, std::min( m_blockSize - 1, id ) );
        return id;
    }

    void getXYZ( int id, int &x, int &y, int &z )
    {
        x = id % m_nx;
        int tempY = id % ( m_nx * m_ny );
        y = tempY / m_nx;
        z = id / ( m_nx * m_ny );
    }
};

#endif /* FIBERS_H_ */
