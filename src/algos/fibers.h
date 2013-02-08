/*
 * fibers.h
 *
 *  Created on: Jan 24, 2013
 *      Author: Ralph Schurade
 */

#ifndef FIBERS_H_
#define FIBERS_H_

#include <QtCore/QVector>

class DatasetFibers;
class DatasetScalar;

class Fibers
{
public:
    Fibers( DatasetFibers* ds );
    virtual ~Fibers();

    DatasetFibers* thinOut();
    DatasetScalar* tractDensity();

private:
    DatasetFibers* m_dataset;

    float getFiberDist( QVector< float >& lhs, QVector< float >& rhs );
    float getDist( float x1, float y1, float z1, float x2, float y2, float z2 );

    QVector<float> mergeFibs( QVector< float >& lhs, QVector< float >& rhs );

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
