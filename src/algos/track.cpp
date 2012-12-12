/*
 * track.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: schurade
 */
#include <omp.h>

#include "../data/datasets/datasettensor.h"

#include "fmath.h"

#include "track.h"

Track::Track( DatasetTensor* ds ) :
    m_dataset( ds ),
    m_nx( 0 ),
    m_ny( 0 ),
    m_nz( 0 ),
    m_dx( 1.0 ),
    m_dy( 1.0 ),
    m_dz( 1.0 ),

    m_minLength( 10 ),
    m_minFA( 0.2 ),
    m_minStartFA( 0.2 ),
    m_stepSize( 1.0 ),
    m_diag( 1.0 ),
    maxStepsInVoxel( 3 ),
    m_smoothness( 0.0 ),
    m_ignoreQForm( false ),
    m_debug( false ),
    m_verbose( true ),
    m_threads( 4 ),
    m_count ( 0 ),
    m_thinOut( false ),
    m_numPoints( 0 ),
    m_numLines( 0 )
{
    m_tensors = ds->getData();
    m_logTensors = ds->getLogData();

    m_nx = ds->getProperty( "nx" ).toInt();
    m_ny = ds->getProperty( "ny" ).toInt();
    m_nz = ds->getProperty( "nz" ).toInt();
    m_dx = ds->getProperty( "dx" ).toFloat();
    m_dy = ds->getProperty( "dy" ).toFloat();
    m_dz = ds->getProperty( "dz" ).toFloat();

    qDebug() << m_dx << m_dy << m_dz;
    m_diag = sqrt( m_dx * m_dx + m_dy * m_dy + m_dz * m_dz );
    maxStepsInVoxel = ( (int) ( m_diag / m_stepSize ) + 1 ) * 2;

    blockSize = m_nx * m_ny * m_nz;

    FMath::evecs( m_tensors, evec1, eval1, evec2, eval2, evec3, eval3 );
    FMath::fa( m_tensors, fa );
}

Track::~Track()
{
}

QVector< QVector< float > > Track::getFibs()
{
    return fibs;
}

int Track::getNumPoints()
{
    return m_numPoints;
}

int Track::getNumLines()
{
    return m_numLines;
}

void Track::startTracking()
{
    srand( time( NULL ) );

    //int v2s = (int)(1.0 / m_stepSize);
    if ( m_verbose )
    {
        qDebug() << "diag: " << m_diag << " msiv: " << maxStepsInVoxel;
        qDebug() << "smoothness: " << m_smoothness;
        qDebug() << "min length: " << m_minLength / m_stepSize;
    }

    trackWholeBrain();
}

void Track::trackWholeBrain()
{
    m_numPoints = 0;
    m_numLines = 0;
    int currentFrame = 0;

    omp_set_num_threads( m_threads );
#pragma omp parallel for
    for ( int z = 0; z < m_nz; ++z )
    {
        QVector<float> fib1;
        QVector<float> fib2;
        QVector<float> fib3;
        QVector<float> fib2r;
        int j = 0;

        for ( int y = 0; y < m_ny; ++y )
        {
            for ( int x = 0; x < m_nx; ++x )
            {
                int id = x + y * m_nx + z * m_ny * m_nx;
                if ( fa[id] >= m_minStartFA )
                {
                    fib1 = track( x, y, z, false );
                    fib2 = track( x, y, z, true );

                    if ( ( fib1.size() + fib2.size() ) / 3 >= m_minLength / m_stepSize )
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

#pragma omp critical
                        {
                            fibs.push_back( fib2r );
                            m_numPoints += fib2r.size() / 3;
                            m_numLines++;
                        }
                    }
                }
                if ( m_thinOut )
                    ++x;
            }
            if ( m_thinOut )
                ++y;
        }
#pragma omp critical
        {
            if ( m_verbose )
            {
                printf( "finished frame %i (%i) / %i \n", currentFrame, z, m_nz - 1 );
            }
            ++currentFrame;
        }
        if ( m_thinOut )
            ++z;
    }

    if ( m_verbose )
    {
        qDebug() << m_numLines << " lines with " << m_numPoints << " points." << endl;
    }
}

QVector<float> Track::track( float xs, float ys, float zs, bool negDir )
{
    QVector<float> result;
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
    int id = getID( x, y, z );

    if ( negDir )
    {
        dirX = evec1[id].x() * -1.0;
        dirY = evec1[id].y() * -1.0;
        dirZ = evec1[id].z() * -1.0;
    }
    else
    {
        dirX = evec1[id].x();
        dirY = evec1[id].y();
        dirZ = evec1[id].z();
    }
    norm = sqrt( dirX * dirX + dirY * dirY + dirZ * dirZ );
    dirX = dirX / norm;
    dirY = dirY / norm;
    dirZ = dirZ / norm;
    float curFA;
    int lc = 0;
    while ( true )
    {
        curFA = getInterpolatedFA( id, x, y, z );

        if ( curFA > m_minFA && ( x == x ) && ( y == y ) && ( z == z ) )
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
    return result;
}

int Track::getID( float x, float y, float z )
{
    int id = (int) ( x / m_dx ) + (int) ( y / m_dy ) * m_nx + (int) ( z / m_dz ) * m_ny * m_nx;

    id = max( (int) 0, min( blockSize - 1, id ) );
    return id;
}

float Track::getInterpolatedFA( int id, float inx, float iny, float inz )
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
    int id_x1y0z0 = min( blockSize - 1, id + 1 );
    int id_x0y1z0 = min( blockSize - 1, id + m_nx );
    int id_x1y1z0 = min( blockSize - 1, id + m_nx + 1 );
    int id_x0y0z1 = min( blockSize - 1, id + m_nx * m_ny );
    int id_x1y0z1 = min( blockSize - 1, id + m_nx * m_ny + 1 );
    int id_x0y1z1 = min( blockSize - 1, id + m_nx * m_ny + m_nx );
    int id_x1y1z1 = min( blockSize - 1, id + m_nx * m_ny + m_nx + 1 );

    float i1;
    float i2;
    float j1;
    float j2;
    float w1;
    float w2;
    float iv;

    i1 = fa[id_x0y0z0] * ( 1.0 - zd ) + fa[id_x0y0z1] * zd;
    i2 = fa[id_x0y1z0] * ( 1.0 - zd ) + fa[id_x0y1z1] * zd;
    j1 = fa[id_x1y0z0] * ( 1.0 - zd ) + fa[id_x1y0z1] * zd;
    j2 = fa[id_x1y1z0] * ( 1.0 - zd ) + fa[id_x1y1z1] * zd;

    w1 = i1 * ( 1.0 - yd ) + i2 * yd;
    w2 = j1 * ( 1.0 - yd ) + j2 * yd;

    iv = w1 * ( 1.0 - xd ) + w2 * xd;

    return iv;
}

Matrix Track::getInterpolatedTensor( int id, float inx, float iny, float inz )
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
    int id_x1y0z0 = min( blockSize - 1, id + 1 );
    int id_x0y1z0 = min( blockSize - 1, id + m_nx );
    int id_x1y1z0 = min( blockSize - 1, id + m_nx + 1 );
    int id_x0y0z1 = min( blockSize - 1, id + m_nx * m_ny );
    int id_x1y0z1 = min( blockSize - 1, id + m_nx * m_ny + 1 );
    int id_x0y1z1 = min( blockSize - 1, id + m_nx * m_ny + m_nx );
    int id_x1y1z1 = min( blockSize - 1, id + m_nx * m_ny + m_nx + 1 );

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

    return expT( iv );
}

Matrix Track::expT( Matrix& t )
{
    double xx, xy, xz, yy, yz, zz;
    double i1, i2, i3, v, s, phi, l1, l2, l3;
    double ev1_x, ev1_y, ev1_z, ev2_x, ev2_y, ev2_z, ev3_x, ev3_y, ev3_z, vec_norm;

    xx = t( 1, 1 );
    xy = t( 1, 2 );
    xz = t( 1, 3 );
    yy = t( 2, 2 );
    yz = t( 2, 3 );
    zz = t( 3, 3 );

    // three invariants of D (dt)
    // i1=l1+l2+l3 (trace)
    // i2=l1*l2+l1*l3+l2*l3
    // i3=l1*l2*l3 (determinante)
    i1 = xx + yy + zz;
    i2 = xx * yy + xx * zz + yy * zz - ( FMath::pow2( xy ) + FMath::pow2( xz ) + FMath::pow2( yz ) );
    i3 = xx * yy * zz + 2. * xy * xz * yz - ( zz * FMath::pow2( xy ) + yy * FMath::pow2( xz ) + xx * FMath::pow2( yz ) );

    v = FMath::pow2( i1 / 3 ) - i2 / 3;
    s = FMath::pow3( i1 / 3 ) - i1 * i2 / 6 + i3 / 2;
    if ( ( v > 0 ) && ( FMath::pow2( s ) < FMath::pow3( v ) ) )
        phi = acos( s / v * sqrt( 1. / v ) ) / 3;
    else
        phi = 0;

    // eigenvalues
    if ( phi != 0 )
    {
        l1 = i1 / 3 + 2 * sqrt( v ) * cos( phi );
        l2 = i1 / 3 - 2 * sqrt( v ) * cos( M_PI / 3. + phi );
        l3 = i1 / 3 - 2 * sqrt( v ) * cos( M_PI / 3. - phi );
    }
    else
        l1 = l2 = l3 = 0.0;
    /*
     eval1[i] = l1;
     eval2[i] = l2;
     eval3[i] = l3;
     */
    // eigenvectors
    ev1_x = ( xy * yz - ( yy - l1 ) * xz ) * ( xz * yz - ( zz - l1 ) * xy );
    ev1_y = ( xz * yz - ( zz - l1 ) * xy ) * ( xz * xy - ( xx - l1 ) * yz );
    ev1_z = ( xy * yz - ( yy - l1 ) * xz ) * ( xz * xy - ( xx - l1 ) * yz );

    ev2_x = ( xy * yz - ( yy - l2 ) * xz ) * ( xz * yz - ( zz - l2 ) * xy );
    ev2_y = ( xz * yz - ( zz - l2 ) * xy ) * ( xz * xy - ( xx - l2 ) * yz );
    ev2_z = ( xy * yz - ( yy - l2 ) * xz ) * ( xz * xy - ( xx - l2 ) * yz );

    ev3_x = ( xy * yz - ( yy - l3 ) * xz ) * ( xz * yz - ( zz - l3 ) * xy );
    ev3_y = ( xz * yz - ( zz - l3 ) * xy ) * ( xz * xy - ( xx - l3 ) * yz );
    ev3_z = ( xy * yz - ( yy - l3 ) * xz ) * ( xz * xy - ( xx - l3 ) * yz );

    vec_norm = sqrt( FMath::pow2( ev1_x ) + FMath::pow2( ev1_y ) + FMath::pow2( ev1_z ) );

    if ( vec_norm > 0 )
    {
        ev1_x = ev1_x / vec_norm;
        ev1_y = ev1_y / vec_norm;
        ev1_z = ev1_z / vec_norm;
    }
    else
        ev1_x = ev1_y = ev1_z = 0.0;

    vec_norm = sqrt( FMath::pow2( ev2_x ) + FMath::pow2( ev2_y ) + FMath::pow2( ev2_z ) );

    if ( vec_norm > 0 )
    {
        ev2_x = ev2_x / vec_norm;
        ev2_y = ev2_y / vec_norm;
        ev2_z = ev2_z / vec_norm;
    }
    else
        ev2_x = ev2_y = ev2_z = 0.0;

    vec_norm = sqrt( FMath::pow2( ev3_x ) + FMath::pow2( ev3_y ) + FMath::pow2( ev3_z ) );

    if ( vec_norm > 0 )
    {
        ev3_x = ev3_x / vec_norm;
        ev3_y = ev3_y / vec_norm;
        ev3_z = ev3_z / vec_norm;
    }
    else
        ev3_x = ev3_y = ev3_z = 0.0;

    Matrix U( 3, 3 );
    DiagonalMatrix D( 3 );

    U( 1, 1 ) = ev1_x;
    U( 2, 1 ) = ev1_y;
    U( 3, 1 ) = ev1_z;
    U( 1, 2 ) = ev2_x;
    U( 2, 2 ) = ev2_y;
    U( 3, 2 ) = ev2_z;
    U( 1, 3 ) = ev3_x;
    U( 2, 3 ) = ev3_y;
    U( 3, 3 ) = ev3_z;
    D( 1 ) = exp( l1 );
    D( 2 ) = exp( l2 );
    D( 3 ) = exp( l3 );
    Matrix expM(3,3);
    expM = U * D * U.t();

    return expM;
}
