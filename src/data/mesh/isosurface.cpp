/*
 * isosurface.cpp
 *
 *  Created on: 09.07.2012
 *      Author: Ralph
 */

#include "trianglemesh.h"
#include "../datasets/datasetscalar.h"

#include "isosurface.h"

int IsoSurface::nX = 1;
int IsoSurface::nY = 1;
int IsoSurface::nZ = 1;

float IsoSurface::dX = 1.0;
float IsoSurface::dY = 1.0;
float IsoSurface::dZ = 1.0;

float IsoSurface::isoLevel = 0.0;

int IsoSurface::nPointsInXDirection = 1;
int IsoSurface::nPointsInSlice = 1;

IsoSurface::IsoSurface(){}
IsoSurface::~IsoSurface(){}

TriangleMesh* IsoSurface::generateSurface( DatasetScalar* ds, float isoValue )
{
    IsoSurface::isoLevel = isoValue;

    nX = ds->getProperty( "nx" ).toInt();
    nY = ds->getProperty( "ny" ).toInt();
    nZ = ds->getProperty( "nz" ).toInt();

    dX = ds->getProperty( "dx" ).toFloat();
    dY = ds->getProperty( "dy" ).toFloat();
    dZ = ds->getProperty( "dz" ).toFloat();

    nPointsInXDirection = ( nX + 1 );
    nPointsInSlice = nPointsInXDirection * ( nY + 1 );

    QVector<float> scalarField = ds->getData();
    QVector<float>* scalarFieldPtr = &scalarField;

    ID2POINT3DID i2pt3idVertices;
    TRIANGLEVECTOR trivec;

    // Generate isosurface.
    for ( unsigned int z = 0; z < nZ; z++ )
    {
        for ( unsigned int y = 0; y < nY; y++ )
        {
            for ( unsigned int x = 0; x < nX; x++ )
            {
                // Calculate table lookup index from those
                // vertices which are below the isolevel.
                unsigned int tableIndex = 0;
                if ( scalarField[z * nPointsInSlice + y * nPointsInXDirection + x] < isoLevel )
                    tableIndex |= 1;
                if ( scalarField[z * nPointsInSlice + ( y + 1 ) * nPointsInXDirection + x] < isoLevel )
                    tableIndex |= 2;
                if ( scalarField[z * nPointsInSlice + ( y + 1 ) * nPointsInXDirection + ( x + 1 )] < isoLevel )
                    tableIndex |= 4;
                if ( scalarField[z * nPointsInSlice + y * nPointsInXDirection + ( x + 1 )] < isoLevel )
                    tableIndex |= 8;
                if ( scalarField[( z + 1 ) * nPointsInSlice + y * nPointsInXDirection + x] < isoLevel )
                    tableIndex |= 16;
                if ( scalarField[( z + 1 ) * nPointsInSlice + ( y + 1 ) * nPointsInXDirection + x] < isoLevel )
                    tableIndex |= 32;
                if ( scalarField[( z + 1 ) * nPointsInSlice + ( y + 1 ) * nPointsInXDirection + ( x + 1 )] < isoLevel )
                    tableIndex |= 64;
                if ( scalarField[( z + 1 ) * nPointsInSlice + y * nPointsInXDirection + ( x + 1 )] < isoLevel )
                    tableIndex |= 128;

                // Now create a triangulation of the isosurface in this
                // cell.
                if ( edgeTable[tableIndex] != 0 )
                {
                    if ( edgeTable[tableIndex] & 8 )
                    {
                        POINT3DID pt = calculateIntersection( x, y, z, 3, scalarFieldPtr );
                        unsigned int id = getEdgeID( x, y, z, 3 );
                        i2pt3idVertices.insert( id, pt );
                    }
                    if ( edgeTable[tableIndex] & 1 )
                    {
                        POINT3DID pt = calculateIntersection( x, y, z, 0, scalarFieldPtr );
                        unsigned int id = getEdgeID( x, y, z, 0 );
                        i2pt3idVertices.insert( id, pt );
                    }
                    if ( edgeTable[tableIndex] & 256 )
                    {
                        POINT3DID pt = calculateIntersection( x, y, z, 8, scalarFieldPtr );
                        unsigned int id = getEdgeID( x, y, z, 8 );
                        i2pt3idVertices.insert( id, pt );
                    }

                    if ( x == nX - 1 )
                    {
                        if ( edgeTable[tableIndex] & 4 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 2, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 2 );
                            i2pt3idVertices.insert( id, pt );
                        }
                        if ( edgeTable[tableIndex] & 2048 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 11, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 11 );
                            i2pt3idVertices.insert( id, pt );
                        }
                    }
                    if ( y == nY - 1 )
                    {
                        if ( edgeTable[tableIndex] & 2 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 1, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 1 );
                            i2pt3idVertices.insert( id, pt );
                        }
                        if ( edgeTable[tableIndex] & 512 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 9, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 9 );
                            i2pt3idVertices.insert( id, pt );
                        }
                    }
                    if ( z == nZ - 1 )
                    {
                        if ( edgeTable[tableIndex] & 16 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 4, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 4 );
                            i2pt3idVertices.insert( id, pt );
                        }
                        if ( edgeTable[tableIndex] & 128 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 7, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 7 );
                            i2pt3idVertices.insert( id, pt );
                        }
                    }
                    if ( ( x == nX - 1 ) && ( y == nY - 1 ) )
                        if ( edgeTable[tableIndex] & 1024 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 10, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 10 );
                            i2pt3idVertices.insert( id, pt );
                        }
                    if ( ( x == nX - 1 ) && ( z == nZ - 1 ) )
                        if ( edgeTable[tableIndex] & 64 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 6, scalarFieldPtr);
                            unsigned int id = getEdgeID( x, y, z, 6 );
                            i2pt3idVertices.insert( id, pt );
                        }
                    if ( ( y == nY - 1 ) && ( z == nZ - 1 ) )
                        if ( edgeTable[tableIndex] & 32 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 5, scalarFieldPtr );
                            unsigned int id = getEdgeID( x, y, z, 5 );
                            i2pt3idVertices.insert( id, pt );
                        }

                    for ( int i = 0; triTable[tableIndex][i] != -1; i += 3 )
                    {
                        TRIANGLE triangle;
                        unsigned int pointID0, pointID1, pointID2;
                        pointID0 = getEdgeID( x, y, z, triTable[tableIndex][i] );
                        pointID1 = getEdgeID( x, y, z, triTable[tableIndex][i + 1] );
                        pointID2 = getEdgeID( x, y, z, triTable[tableIndex][i + 2] );
                        triangle.pointID[0] = pointID0;
                        triangle.pointID[1] = pointID1;
                        triangle.pointID[2] = pointID2;
                        trivec.push_back( triangle );
                    }
                }
            }
        }
    }
    return renameVerticesAndTriangles( i2pt3idVertices, trivec );
}

int IsoSurface::getEdgeID( unsigned int x, unsigned int y, unsigned int z, unsigned int nEdgeNo )
{
    switch ( nEdgeNo )
    {
        case 0:
            return getVertexID( x, y, z ) + 1;
        case 1:
            return getVertexID( x, y + 1, z );
        case 2:
            return getVertexID( x + 1, y, z ) + 1;
        case 3:
            return getVertexID( x, y, z );
        case 4:
            return getVertexID( x, y, z + 1 ) + 1;
        case 5:
            return getVertexID( x, y + 1, z + 1 );
        case 6:
            return getVertexID( x + 1, y, z + 1 ) + 1;
        case 7:
            return getVertexID( x, y, z + 1 );
        case 8:
            return getVertexID( x, y, z ) + 2;
        case 9:
            return getVertexID( x, y + 1, z ) + 2;
        case 10:
            return getVertexID( x + 1, y + 1, z ) + 2;
        case 11:
            return getVertexID( x + 1, y, z ) + 2;
        default:
            // Invalid edge no.
            return -1;
    }
}

unsigned int IsoSurface::getVertexID( unsigned int x, unsigned int y, unsigned int z )
{
    return 3 * ( z * ( y + 1 ) * ( x + 1 ) + y * ( x + 1 ) + x );
}

POINT3DID IsoSurface::calculateIntersection( unsigned int x, unsigned int y, unsigned int z, unsigned int nEdgeNo, QVector<float>* scalarField )
{
    float x1, y1, z1, x2, y2, z2;
    unsigned int v1x = x, v1y = y, v1z = z;
    unsigned int v2x = x, v2y = y, v2z = z;

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

    x1 = v1x * dX;
    y1 = v1y * dY;
    z1 = v1z * dZ;
    x2 = v2x * dX;
    y2 = v2y * dY;
    z2 = v2z * dZ;

    float val1 = (*scalarField)[v1z * nPointsInSlice + v1y * nPointsInXDirection + v1x];
    float val2 = (*scalarField)[v2z * nPointsInSlice + v2y * nPointsInXDirection + v2x];
    POINT3DID intersection = interpolate( x1, y1, z1, x2, y2, z2, val1, val2 );
    intersection.newID = 0;
    return intersection;
}

POINT3DID IsoSurface::interpolate( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2 )
{
    POINT3DID interpolation;
    float mu;

    mu = float( ( IsoSurface::isoLevel - tVal1 ) ) / ( tVal2 - tVal1 );
    interpolation.x = fX1 + mu * ( fX2 - fX1 );
    interpolation.y = fY1 + mu * ( fY2 - fY1 );
    interpolation.z = fZ1 + mu * ( fZ2 - fZ1 );
    interpolation.newID = 0;
    return interpolation;
}

TriangleMesh* IsoSurface::renameVerticesAndTriangles( ID2POINT3DID &i2pt3idVertices, TRIANGLEVECTOR &trivec )
{
    unsigned int nextID = 0;

    TriangleMesh* triMesh = new TriangleMesh( i2pt3idVertices.size(), trivec.size() );

    float xOff = dX * 0.5f;
    float yOff = dY * 0.5f;
    float zOff = dZ * 0.5f;

    // Rename vertices.
    ID2POINT3DID::iterator mapIterator;
    for( mapIterator = i2pt3idVertices.begin(); mapIterator != i2pt3idVertices.end(); ++mapIterator, ++nextID )
    {
        mapIterator.value().newID = nextID;
        triMesh->addVertex( nextID, mapIterator.value().x + xOff, mapIterator.value().y + yOff, mapIterator.value().z + zOff );
    }

    nextID = 0;
    // Now rename triangles.
    TRIANGLEVECTOR::iterator vecIterator;
    for ( vecIterator = trivec.begin(); vecIterator != trivec.end(); ++vecIterator, ++nextID)
    {
        for ( unsigned int i = 0; i < 3; ++i )
        {
            unsigned int newID = i2pt3idVertices[( *vecIterator ).pointID[i]].newID;
            ( *vecIterator ).pointID[i] = newID;
        }
        triMesh->addTriangle( nextID, ( *vecIterator ).pointID[0], ( *vecIterator ).pointID[1], ( *vecIterator ).pointID[2] );
    }


    i2pt3idVertices.clear();
    trivec.clear();

    return triMesh;
}
