/*
 * trackthread.cpp
 *
 * Created on: 25.12.2012
 * @author Ralph Schurade
 */
#include "trackthread.h"
#include "fmath.h"

#include "../gui/gl/glfunctions.h"

#include "time.h"
#include "math.h"

TrackThread::TrackThread( QVector<Matrix>* tensors,
                          QVector<Matrix>* logTensors,
                          QVector<float>* fa,
                          QVector<QVector3D>* evec1,
                          int nx,
                          int ny,
                          int nz,
                          float dx,
                          float dy,
                          float dz,
                          int id,
                          int minLength,
                          float minFA,
                          float minStartFA,
                          float stepSize,
                           float smoothness ) :
    m_tensors( tensors ),
    m_logTensors( logTensors ),
    m_fa( fa ),
    m_evec1( evec1 ),
    m_nx( nx ),
    m_ny( ny ),
    m_nz( nz ),
    m_dx( dx ),
    m_dy( dy ),
    m_dz( dz ),
    m_id( id ),

    m_minLength( minLength ),
    m_minFA( minFA ),
    m_minStartFA( minStartFA ),
    m_stepSize( stepSize ),
    m_diag( 1.0 ),
    maxStepsInVoxel( 3 ),
    m_smoothness( smoothness )
{
    m_diag = sqrt( m_dx * m_dx + m_dy * m_dy + m_dz * m_dz );
    maxStepsInVoxel = ( (int) ( m_diag / m_stepSize ) + 1 ) * 2;
    m_blockSize = m_nx * m_ny * m_nz;
}

TrackThread::~TrackThread()
{
    fibs.clear();
}

QVector< QVector< float > > TrackThread::getFibs()
{
    return fibs;
}

QVector< QVector< float > > TrackThread::getExtras()
{
    return extras;
}

void TrackThread::run()
{
    int numThreads = GLFunctions::idealThreadCount;
    int progressCounter = 0;

    for ( int i = m_id; i < m_blockSize;  i += numThreads )
    {
        QVector<float> fib1;
        QVector<float> fib2;
        QVector<float> fib2r;

        QVector<float> extra1;
        QVector<float> extra2;
        QVector<float> extra2r;

        track( i, false, fib1, extra1 );
        track( i, true, fib2, extra2 );

        int j = 0;
        if ( ( fib1.size() + fib2.size() ) / 3 >= m_minLength )
        {
            fib2r.resize( fib2.size() );
            extra2r.resize( extra2.size() );
            for ( int i = 0; i < fib2.size() / 3; ++i )
            {
                j = i * 3;
                fib2r[j] = fib2[( fib2.size() - 1 ) - ( j + 2 )];
                fib2r[j + 1] = fib2[( fib2.size() - 1 ) - ( j + 1 )];
                fib2r[j + 2] = fib2[( fib2.size() - 1 ) - j];

                extra2r[ j / 3 ] = extra2[( extra2.size() - 1 ) - j/3];
            }

            if ( fib1.size() > 3 )
            {
                fib1.remove( 0 );
                fib1.remove( 0 );
                fib1.remove( 0 );
                extra1.remove( 0 );
                fib2r += fib1;
                extra2r += extra1;
            }

            fibs.push_back( fib2r );
            extras.push_back( extra2r );
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

void TrackThread::track( int id, bool negDir, QVector<float>& result, QVector<float>& extraResult )
{
    int xs = 0;
    int ys = 0;
    int zs = 0;

    getXYZ( id, xs, ys, zs );

    double xx, xy, xz, yy, yz, zz;
    float newDirX, newDirY, newDirZ;
    float dirX, dirY, dirZ, norm;
    int oldId = 0;
    Matrix iT; // interpolated tensor

//  float x = xs * m_dx + 0.5 * m_dx;
//  float y = ys * m_dy + 0.5 * m_dy;
//  float z = zs * m_dz + 0.5 * m_dz;

    float x = xs * m_dx;
    float y = ys * m_dy;
    float z = zs * m_dz;
    float curFA = getInterpolatedFA( id, x, y, z );

    if ( curFA < m_minStartFA )
    {
        return;
    }

    if ( negDir )
    {
        dirX = m_evec1->at( id ).x() * -1.0;
        dirY = m_evec1->at( id ).y() * -1.0;
        dirZ = m_evec1->at( id ).z() * -1.0;
    }
    else
    {
        dirX = m_evec1->at( id ).x();
        dirY = m_evec1->at( id ).y();
        dirZ = m_evec1->at( id ).z();
    }
    norm = sqrt( dirX * dirX + dirY * dirY + dirZ * dirZ );
    dirX = dirX / norm;
    dirY = dirY / norm;
    dirZ = dirZ / norm;

    int lc = 0;
    while ( true )
    {
        curFA = getInterpolatedFA( id, x, y, z );

        if ( curFA > m_minFA && ( x == x ) && ( y == y ) && ( z == z ) )
        {
            result.push_back( x );
            result.push_back( y );
            result.push_back( z );

            extraResult.push_back( curFA );
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

        iT = getInterpolatedTensor( id, x, y, z );

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

int TrackThread::getID( float x, float y, float z )
{
    int id = (int) ( x / m_dx ) + (int) ( y / m_dy ) * m_nx + (int) ( z / m_dz ) * m_ny * m_nx;

    id = max( (int) 0, min( m_blockSize - 1, id ) );
    return id;
}

void TrackThread::getXYZ( int id, int &x, int &y, int &z )
{
    x = id % m_nx;
    int tempY = id % ( m_nx * m_ny );
    y = tempY / m_nx;
    z = id / ( m_nx * m_ny );
}

float TrackThread::getInterpolatedFA( int id, float inx, float iny, float inz )
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

    i1 = m_fa->at( id_x0y0z0 ) * ( 1.0 - zd ) + m_fa->at( id_x0y0z1 ) * zd;
    i2 = m_fa->at( id_x0y1z0 ) * ( 1.0 - zd ) + m_fa->at( id_x0y1z1 ) * zd;
    j1 = m_fa->at( id_x1y0z0 ) * ( 1.0 - zd ) + m_fa->at( id_x1y0z1 ) * zd;
    j2 = m_fa->at( id_x1y1z0 ) * ( 1.0 - zd ) + m_fa->at( id_x1y1z1 ) * zd;

    w1 = i1 * ( 1.0 - yd ) + i2 * yd;
    w2 = j1 * ( 1.0 - yd ) + j2 * yd;

    iv = w1 * ( 1.0 - xd ) + w2 * xd;

    return iv;
}

Matrix TrackThread::getInterpolatedTensor( int id, float inx, float iny, float inz )
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
            i1( i, j ) = m_logTensors->at( id_x0y0z0 )( i, j ) * ( 1.0 - zd ) + m_logTensors->at( id_x0y0z1 )( i, j ) * zd;
            i2( i, j ) = m_logTensors->at( id_x0y1z0 )( i, j ) * ( 1.0 - zd ) + m_logTensors->at( id_x0y1z1 )( i, j ) * zd;
            j1( i, j ) = m_logTensors->at( id_x1y0z0 )( i, j ) * ( 1.0 - zd ) + m_logTensors->at( id_x1y0z1 )( i, j ) * zd;
            j2( i, j ) = m_logTensors->at( id_x1y1z0 )( i, j ) * ( 1.0 - zd ) + m_logTensors->at( id_x1y1z1 )( i, j ) * zd;
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
