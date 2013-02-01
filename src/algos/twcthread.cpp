/*
 * twcthread.cpp
 *
 *  Created on: Jan 28, 2013
 *      Author: schurade
 */
#include "twcthread.h"

#include "fmath.h"

#include "time.h"

TWCThread::TWCThread( int id,
                        QVector<float>* mask,
                        QVector<Matrix>* logtensors1,
                        QVector<Matrix>* logtensors2,
                        QVector<Matrix>* logtensors3,
                        QVector<QVector3D>* evec1,
                        QVector<QVector3D>* evec2,
                        QVector<QVector3D>* evec3,
                        int nx, int ny, int nz, float dx, float dy, float dz ) :
    m_id( id ),
    m_mask( mask ),
    m_nx( nx ),
    m_ny( ny ),
    m_nz( nz ),
    m_dx( dx ),
    m_dy( dy ),
    m_dz( dz ),
    m_minLength( 20 ),
    m_stepSize( 1.0 ),
    m_diag( 1.0 ),
    maxStepsInVoxel( 5 ),
    m_smoothness( 0.0 )
{
    m_logTensors.push_back( logtensors1 );
    m_logTensors.push_back( logtensors2 );
    m_logTensors.push_back( logtensors3 );
    m_evecs.push_back( evec1 );
    m_evecs.push_back( evec2 );
    m_evecs.push_back( evec3 );

    m_blockSize = nx * ny * nz;
}

TWCThread::~TWCThread()
{
}

QVector< QVector< float > > TWCThread::getFibs()
{
    return fibs;
}

void TWCThread::run()
{
    int numThreads = QThread::idealThreadCount();
    int progressCounter = 0;

    for ( int i = m_id; i < m_blockSize;  i += numThreads )
    {
        if ( m_mask->at( i ) < 0.2 )
        {
            continue;
        }

        QVector<float> fib1;
        QVector<float> fib2;
        QVector<float> fib3;
        QVector<float> fib2r;
        int j = 0;

        track( i, false, fib1 );
        track( i, true, fib2 );

        if ( ( fib1.size() + fib2.size() ) / 3 >= m_minLength )
        {
            fib2r.resize( fib2.size() );
            for ( int i = 0; i < fib2.size() / 3; ++i )
            {
                j = i * 3;
                fib2r[j] = fib2[( fib2.size() - 1 ) - ( j + 2 )];
                fib2r[j + 1] = fib2[( fib2.size() - 1 ) - ( j + 1 )];
                fib2r[j + 2] = fib2[( fib2.size() - 1 ) - j];
            }

            if ( fib1.size() > 3 )
            {
                fib1.remove( 0 );
                fib1.remove( 0 );
                fib1.remove( 0 );
                fib2r += fib1;
            }

            fibs.push_back( fib2r );
        }
        ++progressCounter;
        if ( progressCounter == 100 )
        {
            emit( progress() );
            progressCounter = 0;
        }
    }
    emit( finished() );
}

void TWCThread::track( int id, bool negDir, QVector<float>& result )
{
    int xs = 0;
    int ys = 0;
    int zs = 0;
    getXYZ( id, xs, ys, zs );

    double xx, xy, xz, yy, yz, zz;
    float newDirX, newDirY, newDirZ;

    int oldId = 0;
    Matrix iT; // interpolated tensor

    // getStart direction
    float dirX, dirY, dirZ;
    if ( negDir )
    {
        dirX = m_evecs[0]->at( id ).x() * -1.0;
        dirY = m_evecs[0]->at( id ).y() * -1.0;
        dirZ = m_evecs[0]->at( id ).z() * -1.0;
    }
    else
    {
        dirX = m_evecs[0]->at( id ).x();
        dirY = m_evecs[0]->at( id ).y();
        dirZ = m_evecs[0]->at( id ).z();
    }

    float norm = sqrt( dirX * dirX + dirY * dirY + dirZ * dirZ );
    dirX = dirX / norm;
    dirY = dirY / norm;
    dirZ = dirZ / norm;

    float x = xs * m_dx;
    float y = ys * m_dy;
    float z = zs * m_dz;
    float curFA = getInterpolatedFA( id, x, y, z );

    int lc = 0;
    while ( true )
    {
        curFA = getInterpolatedFA( id, x, y, z );

        if ( curFA > 0.2 && ( x == x ) && ( y == y ) && ( z == z ) )
        {
            result.push_back( x );
            result.push_back( y );
            result.push_back( z );
        }
        else
        {
            break;
        }

        x += dirX * m_stepSize;
        y += dirY * m_stepSize;
        z += dirZ * m_stepSize;

        id = getID( x, y, z );

        if ( oldId == id )
        {
            ++lc;
            if ( lc > maxStepsInVoxel )
            {
                //printf( "stop tracking, fiber never left voxel \n" );
                break;
            }
        }
        else
            lc = 0;

        iT = getInterpolatedTensor( id, x, y, z, dirX, dirY, dirZ );

        xx = iT( 1, 1 );
        xy = iT( 1, 2 );
        xz = iT( 1, 3 );
        yy = iT( 2, 2 );
        yz = iT( 2, 3 );
        zz = iT( 3, 3 );

        // dir = tensor(xyz) * dir;
        newDirX = xx * dirX + xy * dirY + xz * dirZ;
        newDirY = xy * dirX + yy * dirY + yz * dirZ;
        newDirZ = xz * dirX + yz * dirY + zz * dirZ;

        norm = sqrt( newDirX * newDirX + newDirY * newDirY + newDirZ * newDirZ );
        newDirX = newDirX / norm;
        newDirY = newDirY / norm;
        newDirZ = newDirZ / norm;

        newDirX = m_smoothness * dirX + ( 1.0 - m_smoothness ) * newDirX;
        newDirY = m_smoothness * dirY + ( 1.0 - m_smoothness ) * newDirY;
        newDirZ = m_smoothness * dirZ + ( 1.0 - m_smoothness ) * newDirZ;

        norm = sqrt( newDirX * newDirX + newDirY * newDirY + newDirZ * newDirZ );
        dirX = newDirX / norm;
        dirY = newDirY / norm;
        dirZ = newDirZ / norm;
        oldId = id;
    }
}

float TWCThread::getInterpolatedFA( int& id, float& inx, float& iny, float& inz )
{
    float x = inx / m_dx;
    float y = iny / m_dy;
    float z = inz / m_dz;

    int x0 = (int) x;
    int y0 = (int) y;
    int z0 = (int) z;

    float xd = x - x0;
    float yd = y - y0;
    float zd = z - z0;

    int id_x0y0z0 = id;
    int id_x1y0z0 = min( m_blockSize - 1, id + 1 );
    int id_x0y1z0 = min( m_blockSize - 1, id + m_nx );
    int id_x1y1z0 = min( m_blockSize - 1, id + m_nx + 1 );
    int id_x0y0z1 = min( m_blockSize - 1, id + m_nx * m_ny );
    int id_x1y0z1 = min( m_blockSize - 1, id + m_nx * m_ny + 1 );
    int id_x0y1z1 = min( m_blockSize - 1, id + m_nx * m_ny + m_nx );
    int id_x1y1z1 = min( m_blockSize - 1, id + m_nx * m_ny + m_nx + 1 );

    float i1;
    float i2;
    float j1;
    float j2;
    float w1;
    float w2;
    float iv;

    i1 = m_mask->at( id_x0y0z0 ) * ( 1.0 - zd ) + m_mask->at( id_x0y0z1 ) * zd;
    i2 = m_mask->at( id_x0y1z0 ) * ( 1.0 - zd ) + m_mask->at( id_x0y1z1 ) * zd;
    j1 = m_mask->at( id_x1y0z0 ) * ( 1.0 - zd ) + m_mask->at( id_x1y0z1 ) * zd;
    j2 = m_mask->at( id_x1y1z0 ) * ( 1.0 - zd ) + m_mask->at( id_x1y1z1 ) * zd;

    w1 = i1 * ( 1.0 - yd ) + i2 * yd;
    w2 = j1 * ( 1.0 - yd ) + j2 * yd;

    iv = w1 * ( 1.0 - xd ) + w2 * xd;

    return iv;
}

Matrix TWCThread::getInterpolatedTensor( int &id, float &inx, float &iny, float &inz, float &dirX, float &dirY, float &dirZ )
{
    float x = inx / m_dx;
    float y = iny / m_dy;
    float z = inz / m_dz;

    int x0 = (int) x;
    int y0 = (int) y;
    int z0 = (int) z;

    float xd = x - x0;
    float yd = y - y0;
    float zd = z - z0;

    int id_x0y0z0 = id;
    int id_x1y0z0 = min( m_blockSize - 1, id + 1 );
    int id_x0y1z0 = min( m_blockSize - 1, id + m_nx );
    int id_x1y1z0 = min( m_blockSize - 1, id + m_nx + 1 );
    int id_x0y0z1 = min( m_blockSize - 1, id + m_nx * m_ny );
    int id_x1y0z1 = min( m_blockSize - 1, id + m_nx * m_ny + 1 );
    int id_x0y1z1 = min( m_blockSize - 1, id + m_nx * m_ny + m_nx );
    int id_x1y1z1 = min( m_blockSize - 1, id + m_nx * m_ny + m_nx + 1 );

    QVector<Matrix>* lt_x0y0z0 = testAngle( id_x0y0z0, dirX, dirY, dirZ );
    QVector<Matrix>* lt_x1y0z0 = testAngle( id_x1y0z0, dirX, dirY, dirZ );
    QVector<Matrix>* lt_x0y1z0 = testAngle( id_x0y1z0, dirX, dirY, dirZ );
    QVector<Matrix>* lt_x1y1z0 = testAngle( id_x1y1z0, dirX, dirY, dirZ );
    QVector<Matrix>* lt_x0y0z1 = testAngle( id_x0y0z1, dirX, dirY, dirZ );
    QVector<Matrix>* lt_x1y0z1 = testAngle( id_x1y0z1, dirX, dirY, dirZ );
    QVector<Matrix>* lt_x0y1z1 = testAngle( id_x0y1z1, dirX, dirY, dirZ );
    QVector<Matrix>* lt_x1y1z1 = testAngle( id_x1y1z1, dirX, dirY, dirZ );

    Matrix i1( 3, 3 );
    Matrix i2( 3, 3 );
    Matrix j1( 3, 3 );
    Matrix j2( 3, 3 );
    Matrix w1( 3, 3 );
    Matrix w2( 3, 3 );
    Matrix iv( 3, 3 );

    for( int i = 1; i < 4; ++i )
    {
        for ( int j = 1; j < 4; ++j )
        {
            i1( i, j ) = lt_x0y0z0->at( id_x0y0z0 )( i, j ) * ( 1.0 - zd ) + lt_x0y0z1->at( id_x0y0z1 )( i, j ) * zd;
            i2( i, j ) = lt_x0y1z0->at( id_x0y1z0 )( i, j ) * ( 1.0 - zd ) + lt_x0y1z1->at( id_x0y1z1 )( i, j ) * zd;
            j1( i, j ) = lt_x1y0z0->at( id_x1y0z0 )( i, j ) * ( 1.0 - zd ) + lt_x1y0z1->at( id_x1y0z1 )( i, j ) * zd;
            j2( i, j ) = lt_x1y1z0->at( id_x1y1z0 )( i, j ) * ( 1.0 - zd ) + lt_x1y1z1->at( id_x1y1z1 )( i, j ) * zd;
        }
    }

    for( int i = 1; i < 4; ++i )
    {
        for ( int j = 1; j < 4; ++j )
        {
            w1( i, j ) = i1( i, j ) * ( 1.0 - yd ) + i2( i, j ) * yd;
            w2( i, j ) = j1( i, j ) * ( 1.0 - yd ) + j2( i, j ) * yd;
        }
    }

    for( int i = 1; i < 4; ++i )
    {
        for ( int j = 1; j < 4; ++j )
        {
            iv( i, j ) = w1( i, j ) * ( 1.0 - xd ) + w2( i, j ) * xd;
        }
    }

    return FMath::expT( iv );
}

QVector<Matrix>* TWCThread::testAngle( int &id, float &dirX, float &dirY, float &dirZ )
{
    float dotP0= fabs( dirX * m_evecs[0]->at( id ).x() + dirY * m_evecs[0]->at( id ).y() + dirZ * m_evecs[0]->at( id ).z() );
    float dotP1= fabs( dirX * m_evecs[1]->at( id ).x() + dirY * m_evecs[1]->at( id ).y() + dirZ * m_evecs[1]->at( id ).z() );
    float dotP2= fabs( dirX * m_evecs[2]->at( id ).x() + dirY * m_evecs[2]->at( id ).y() + dirZ * m_evecs[2]->at( id ).z() );

    if ( dotP0 >= dotP1 && dotP0 >= dotP2 )
    {
        return m_logTensors[0];
    }
    if( dotP1 >= dotP2 )
    {
        return m_logTensors[1];
    }
    return m_logTensors[2];
}
