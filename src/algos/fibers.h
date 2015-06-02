/*
 * fibers.h
 *
 * Created on: Jan 24, 2013
 * @author Ralph Schurade
 */

#ifndef FIBERS_H_
#define FIBERS_H_

#include "fib.h"

#include "../thirdparty/nifti/nifti1_io.h"

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
    DatasetFibers* unfold();

private:
    DatasetFibers* m_dataset;

    Fib mergeFibs( Fib& lhs, Fib& rhs );
    nifti_image* createHeader( int dim );

    std::vector<Fib> unfolder_sphere( std::vector<Fib>& fibs );



    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;
    float m_ax;
    float m_ay;
    float m_az;
    int m_blockSize;

    int getID( float x, float y, float z )
    {
        int id = (int) ( ( x - m_ax ) / m_dx ) + (int) ( ( y - m_ay ) / m_dy ) * m_nx + (int) ( ( z - m_az ) / m_dz ) * m_ny * m_nx;

        id = std::max( (int) 0, std::min( m_blockSize - 1, id ) );
        return id;
    }

    typedef struct
    {
        float x,y,z;
    } float3D;

    typedef struct
    {
        int a,b,c;
    } int3D;

    // Vector algebra
    float norm3D(float3D a)
    {
        return sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
    }
    float3D add3D(float3D a, float3D b)
    {
        return (float3D){a.x+b.x,a.y+b.y,a.z+b.z};
    }
    float3D sub3D(float3D a, float3D b)
    {
        return (float3D){a.x-b.x,a.y-b.y,a.z-b.z};
    }
    float3D sca3D(float3D a, float t)
    {
        return (float3D){a.x*t,a.y*t,a.z*t};
    }
    float3D cross3D(float3D a, float3D b)
    {
        return (float3D){a.y*b.z-a.z*b.y,b.x*a.z-b.z*a.x,a.x*b.y-a.y*b.x};
    }


};

#endif /* FIBERS_H_ */
