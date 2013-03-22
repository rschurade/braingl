/*
 * isosurfacethread.cpp
 *
 * Created on: 28.12.2012
 * @author Ralph Schurade
 */
#include "isosurfacethread.h"

#include "../../gui/gl/glfunctions.h"

#include <QDebug>

IsoSurfaceThread::IsoSurfaceThread( QVector<float>* scalarField, QMutex* mutex, ID2POINT3DID* i2pt3idVertices, TRIANGLEVECTOR* trivecTriangles,
                                    int nx, int ny, int nz, float dx, float dy, float dz, int id ) :
    m_scalarField( scalarField ),
    m_mutex( mutex ),
    m_i2pt3idVertices( i2pt3idVertices ),
    m_trivecTriangles( trivecTriangles ),
    m_nX( nx ),
    m_nY( ny ),
    m_nZ( nz ),
    m_dX( dx ),
    m_dY( dy ),
    m_dZ( dz ),
    m_id( id ),
    m_isoLevel( 0 )
{
    m_nPointsInXDirection = ( m_nX + 1 );
    m_nPointsInSlice = m_nPointsInXDirection * ( m_nY + 1 );
}

IsoSurfaceThread::~IsoSurfaceThread()
{
}

void IsoSurfaceThread::setIsoLevel( float isoLevel )
{
    m_isoLevel = isoLevel;
}

void IsoSurfaceThread::getXYZ( int id, int &x, int &y, int &z )
{
    x = id % ( m_nX + 1 );
    int tempY = id % ( ( m_nX + 1 ) * ( m_nY + 1 ) );
    y = tempY / ( m_nX + 1 );
    z = id / ( ( m_nX + 1 ) * ( m_nY + 1 ) );
}


void IsoSurfaceThread::run()
{
    int numThreads = GLFunctions::idealThreadCount;

    int chunkSize = m_scalarField->size() / numThreads;

    int begin = m_id * chunkSize;
    int end = m_id * chunkSize + chunkSize;

    if ( m_id == numThreads - 1 )
    {
        end = m_scalarField->size() - ( ( m_nX + 1 ) * ( m_nY + 1 ) );
    }

    int x;
    int y;
    int z;

    // Generate isosurface.
    for ( int k = begin; k < end; ++k )
    {
        getXYZ( k, x, y, z );
        if ( x == m_nX ) continue;
        if ( y == m_nY ) continue;

        // Calculate table lookup index from those
        // vertices which are below the isolevel.
        unsigned int tableIndex = 0;
        if ( m_scalarField->at( z * m_nPointsInSlice + y * m_nPointsInXDirection + x ) < m_isoLevel )
            tableIndex |= 1;
        if ( m_scalarField->at( z * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + x ) < m_isoLevel )
            tableIndex |= 2;
        if ( m_scalarField->at( z * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + ( x + 1 ) ) < m_isoLevel )
            tableIndex |= 4;
        if ( m_scalarField->at( z * m_nPointsInSlice + y * m_nPointsInXDirection + ( x + 1 ) ) < m_isoLevel )
            tableIndex |= 8;
        if ( m_scalarField->at( ( z + 1 ) * m_nPointsInSlice + y * m_nPointsInXDirection + x ) < m_isoLevel )
            tableIndex |= 16;
        if ( m_scalarField->at( ( z + 1 ) * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + x ) < m_isoLevel )
            tableIndex |= 32;
        if ( m_scalarField->at( ( z + 1 ) * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + ( x + 1 ) ) < m_isoLevel )
            tableIndex |= 64;
        if ( m_scalarField->at( ( z + 1 ) * m_nPointsInSlice + y * m_nPointsInXDirection + ( x + 1 ) ) < m_isoLevel )
            tableIndex |= 128;

        // Now create a triangulation of the isosurface in this
        // cell.
        if ( edgeTable[ tableIndex ] != 0 )
        {
            if ( edgeTable[ tableIndex ] & 8 )
            {
                POINT3DID pt = calculateIntersection( x, y, z, 3 );
                unsigned int id = getEdgeID( x, y, z, 3 );
                QMutexLocker locker( m_mutex );
                m_i2pt3idVertices->insert( id, pt );
                locker.unlock();
            }
            if ( edgeTable[ tableIndex ] & 1 )
            {
                POINT3DID pt = calculateIntersection( x, y, z, 0 );
                unsigned int id = getEdgeID( x, y, z, 0 );
                QMutexLocker locker( m_mutex );
                m_i2pt3idVertices->insert( id, pt );
                locker.unlock();
            }
            if ( edgeTable[ tableIndex ] & 256 )
            {
                POINT3DID pt = calculateIntersection( x, y, z, 8 );
                unsigned int id = getEdgeID( x, y, z, 8 );
                QMutexLocker locker( m_mutex );
                m_i2pt3idVertices->insert( id, pt );
                locker.unlock();
            }

            if ( x == m_nX - 1 )
            {
                if ( edgeTable[ tableIndex ] & 4 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 2 );
                    unsigned int id = getEdgeID( x, y, z, 2 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
                if ( edgeTable[ tableIndex ] & 2048 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 11 );
                    unsigned int id = getEdgeID( x, y, z, 11 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
            }
            if ( y == m_nY - 1 )
            {
                if ( edgeTable[ tableIndex ] & 2 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 1 );
                    unsigned int id = getEdgeID( x, y, z, 1 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
                if ( edgeTable[ tableIndex ] & 512 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 9 );
                    unsigned int id = getEdgeID( x, y, z, 9 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
            }
            if ( z == m_nZ - 1 )
            {
                if ( edgeTable[ tableIndex ] & 16 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 4 );
                    unsigned int id = getEdgeID( x, y, z, 4 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
                if ( edgeTable[ tableIndex ] & 128 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 7 );
                    unsigned int id = getEdgeID( x, y, z, 7 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
            }
            if ( ( x == m_nX - 1 ) && ( y == m_nY - 1 ) )
                if ( edgeTable[ tableIndex ] & 1024 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 10 );
                    unsigned int id = getEdgeID( x, y, z, 10 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
            if ( ( x == m_nX - 1 ) && ( z == m_nZ - 1 ) )
                if ( edgeTable[ tableIndex ] & 64 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 6 );
                    unsigned int id = getEdgeID( x, y, z, 6 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }
            if ( ( y == m_nY - 1 ) && ( z == m_nZ - 1 ) )
                if ( edgeTable[ tableIndex ] & 32 )
                {
                    POINT3DID pt = calculateIntersection( x, y, z, 5 );
                    unsigned int id = getEdgeID( x, y, z, 5 );
                    QMutexLocker locker( m_mutex );
                    m_i2pt3idVertices->insert( id, pt );
                    locker.unlock();
                }

            for ( int i = 0; triTable[ tableIndex ][ i ] != -1; i += 3 )
            {
                TRIANGLE triangle;
                unsigned int pointID0, pointID1, pointID2;
                pointID0 = getEdgeID( x, y, z, triTable[ tableIndex ][ i ] );
                pointID1 = getEdgeID( x, y, z, triTable[ tableIndex ][ i + 1 ] );
                pointID2 = getEdgeID( x, y, z, triTable[ tableIndex ][ i + 2 ] );
                triangle.pointID[ 0 ] = pointID0;
                triangle.pointID[ 1 ] = pointID1;
                triangle.pointID[ 2 ] = pointID2;
                QMutexLocker locker( m_mutex );
                m_trivecTriangles->push_back( triangle );
                locker.unlock();
            }
        }
    }
}

int IsoSurfaceThread::getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo )
{
    switch ( nEdgeNo )
    {
        case 0:
            return getVertexID( nX, nY, nZ ) + 1;
        case 1:
            return getVertexID( nX, nY + 1, nZ );
        case 2:
            return getVertexID( nX + 1, nY, nZ ) + 1;
        case 3:
            return getVertexID( nX, nY, nZ );
        case 4:
            return getVertexID( nX, nY, nZ + 1 ) + 1;
        case 5:
            return getVertexID( nX, nY + 1, nZ + 1 );
        case 6:
            return getVertexID( nX + 1, nY, nZ + 1 ) + 1;
        case 7:
            return getVertexID( nX, nY, nZ + 1 );
        case 8:
            return getVertexID( nX, nY, nZ ) + 2;
        case 9:
            return getVertexID( nX, nY + 1, nZ ) + 2;
        case 10:
            return getVertexID( nX + 1, nY + 1, nZ ) + 2;
        case 11:
            return getVertexID( nX + 1, nY, nZ ) + 2;
        default:
            // Invalid edge no.
            return -1;
    }
}

unsigned int IsoSurfaceThread::getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ )
{
    return 3 * ( nZ * ( m_nY + 1 ) * ( m_nX + 1 ) + nY * ( m_nX + 1 ) + nX );
}

POINT3DID IsoSurfaceThread::calculateIntersection( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo )
{
    float x1, y1, z1, x2, y2, z2;
    unsigned int v1x = nX, v1y = nY, v1z = nZ;
    unsigned int v2x = nX, v2y = nY, v2z = nZ;

    switch ( nEdgeNo )
    {
        case 0:
            v2y += 1;
            break;
        case 1:
            v1y += 1;
            v2x += 1;
            v2y += 1;
            break;
        case 2:
            v1x += 1;
            v1y += 1;
            v2x += 1;
            break;
        case 3:
            v1x += 1;
            break;
        case 4:
            v1z += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 5:
            v1y += 1;
            v1z += 1;
            v2x += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 6:
            v1x += 1;
            v1y += 1;
            v1z += 1;
            v2x += 1;
            v2z += 1;
            break;
        case 7:
            v1x += 1;
            v1z += 1;
            v2z += 1;
            break;
        case 8:
            v2z += 1;
            break;
        case 9:
            v1y += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 10:
            v1x += 1;
            v1y += 1;
            v2x += 1;
            v2y += 1;
            v2z += 1;
            break;
        case 11:
            v1x += 1;
            v2x += 1;
            v2z += 1;
            break;
    }

    x1 = v1x * m_dX;
    y1 = v1y * m_dY;
    z1 = v1z * m_dZ;
    x2 = v2x * m_dX;
    y2 = v2y * m_dY;
    z2 = v2z * m_dZ;

    float val1 = m_scalarField->at( v1z * m_nPointsInSlice + v1y * m_nPointsInXDirection + v1x );
    float val2 = m_scalarField->at( v2z * m_nPointsInSlice + v2y * m_nPointsInXDirection + v2x );
    POINT3DID intersection = interpolate( x1, y1, z1, x2, y2, z2, val1, val2 );
    intersection.newID = 0;
    return intersection;
}

POINT3DID IsoSurfaceThread::interpolate( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2 )
{
    POINT3DID interpolation;
    float mu;

    mu = float( ( m_isoLevel - tVal1 ) ) / ( tVal2 - tVal1 );
    interpolation.x = fX1 + mu * ( fX2 - fX1 );
    interpolation.y = fY1 + mu * ( fY2 - fY1 );
    interpolation.z = fZ1 + mu * ( fZ2 - fZ1 );
    interpolation.newID = 0;
    return interpolation;
}
