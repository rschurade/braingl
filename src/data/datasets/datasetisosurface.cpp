/*
 * datasetisosurface.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: schurade
 */
#include <QtCore/QString>

#include "../datastore.h"
#include "../../gui/gl/meshrenderer.h"

#include "../mesh/trianglemesh2.h"

#include "datasetscalar.h"
#include "datasetisosurface.h"

DatasetIsosurface::DatasetIsosurface( DatasetScalar* ds ) :
    Dataset( QString("isosurface"), FNDT_MESH_ISOSURFACE ),
    m_mesh( 0 ),
    m_renderer( 0 ),
    m_oldIsoValue( -1 )
{
    m_scalarField = ds->getData();
    setProperties( ds->getProperties() );
    setProperty( "nt", 0 );
    setProperty( "createdBy", FNALGO_ISOSURFACE );
    setProperty( "type", FNDT_MESH_ISOSURFACE );
    setProperty( "name", "isosurface" );
    setProperty( "isoValue", 200 );
}

DatasetIsosurface::~DatasetIsosurface()
{
}

void DatasetIsosurface::draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* dataStore )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh );
        m_renderer->setModel( dataStore );
        m_renderer->init();
    }

    if ( m_oldIsoValue != getProperty( "isoValue").toFloat() )
    {
        delete m_mesh;
        m_mesh = generateSurface( getProperty( "isoValue").toFloat() );
        m_oldIsoValue = getProperty( "isoValue").toFloat();
        m_renderer->setMesh( m_mesh );
    }

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetIsosurface::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}

TriangleMesh2* DatasetIsosurface::generateSurface( float isoValue )
{
    //qDebug() << "start generating iso surface";
    m_isoLevel = isoValue;

    m_nX = getProperty( "nx" ).toInt() - 1;
    m_nY = getProperty( "ny" ).toInt() - 1;
    m_nZ = getProperty( "nz" ).toInt() - 1;

    m_dX = getProperty( "dx" ).toFloat();
    m_dY = getProperty( "dy" ).toFloat();
    m_dZ = getProperty( "dz" ).toFloat();

    m_nPointsInXDirection = ( m_nX + 1 );
    m_nPointsInSlice = m_nPointsInXDirection * ( m_nY + 1 );

    ID2POINT3DID i2pt3idVertices;
    TRIANGLEVECTOR trivec;

    // Generate isosurface.
    for ( int z = 0; z < m_nZ; ++z )
    {
        for ( int y = 0; y < m_nY; ++y )
        {
            for ( int x = 0; x < m_nX; ++x )
            {
                // Calculate table lookup index from those
                // vertices which are below the isolevel.
                int tableIndex = 0;
                if ( m_scalarField[z * m_nPointsInSlice + y * m_nPointsInXDirection + x] < m_isoLevel )
                    tableIndex |= 1;
                if ( m_scalarField[z * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + x] < m_isoLevel )
                    tableIndex |= 2;
                if ( m_scalarField[z * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + ( x + 1 )] < m_isoLevel )
                    tableIndex |= 4;
                if ( m_scalarField[z * m_nPointsInSlice + y * m_nPointsInXDirection + ( x + 1 )] < m_isoLevel )
                    tableIndex |= 8;
                if ( m_scalarField[( z + 1 ) * m_nPointsInSlice + y * m_nPointsInXDirection + x] < m_isoLevel )
                    tableIndex |= 16;
                if ( m_scalarField[( z + 1 ) * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + x] < m_isoLevel )
                    tableIndex |= 32;
                if ( m_scalarField[( z + 1 ) * m_nPointsInSlice + ( y + 1 ) * m_nPointsInXDirection + ( x + 1 )] < m_isoLevel )
                    tableIndex |= 64;
                if ( m_scalarField[( z + 1 ) * m_nPointsInSlice + y * m_nPointsInXDirection + ( x + 1 )] < m_isoLevel )
                    tableIndex |= 128;

                // Now create a triangulation of the isosurface in this
                // cell.
                if ( edgeTable[tableIndex] != 0 )
                {
                    if ( edgeTable[tableIndex] & 8 )
                    {
                        POINT3DID pt = calculateIntersection( x, y, z, 3 );
                        int id = getEdgeID( x, y, z, 3 );
                        i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                    }
                    if ( edgeTable[tableIndex] & 1 )
                    {
                        POINT3DID pt = calculateIntersection( x, y, z, 0 );
                        int id = getEdgeID( x, y, z, 0 );
                        i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                    }
                    if ( edgeTable[tableIndex] & 256 )
                    {
                        POINT3DID pt = calculateIntersection( x, y, z, 8 );
                        int id = getEdgeID( x, y, z, 8 );
                        i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                    }

                    if ( x == m_nX - 1 )
                    {
                        if ( edgeTable[tableIndex] & 4 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 2 );
                            int id = getEdgeID( x, y, z, 2 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                        if ( edgeTable[tableIndex] & 2048 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 11 );
                            int id = getEdgeID( x, y, z, 11 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                    }
                    if ( y == m_nY - 1 )
                    {
                        if ( edgeTable[tableIndex] & 2 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 1 );
                            int id = getEdgeID( x, y, z, 1 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                        if ( edgeTable[tableIndex] & 512 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 9 );
                            int id = getEdgeID( x, y, z, 9 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                    }
                    if ( z == m_nZ - 1 )
                    {
                        if ( edgeTable[tableIndex] & 16 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 4 );
                            int id = getEdgeID( x, y, z, 4 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                        if ( edgeTable[tableIndex] & 128 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 7 );
                            int id = getEdgeID( x, y, z, 7 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                    }
                    if ( ( x == m_nX - 1 ) && ( y == m_nY - 1 ) )
                        if ( edgeTable[tableIndex] & 1024 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 10 );
                            int id = getEdgeID( x, y, z, 10 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                    if ( ( x == m_nX - 1 ) && ( z == m_nZ - 1 ) )
                        if ( edgeTable[tableIndex] & 64 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 6 );
                            int id = getEdgeID( x, y, z, 6 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }
                    if ( ( y == m_nY - 1 ) && ( z == m_nZ - 1 ) )
                        if ( edgeTable[tableIndex] & 32 )
                        {
                            POINT3DID pt = calculateIntersection( x, y, z, 5 );
                            int id = getEdgeID( x, y, z, 5 );
                            i2pt3idVertices.insert( ID2POINT3DID::value_type( id, pt ) );
                        }

                    for ( int i = 0; triTable[tableIndex][i] != -1; i += 3 )
                    {
                        TRIANGLE triangle;
                        int pointID0, pointID1, pointID2;
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
    //qDebug() << "finished generating iso surface";
    return renameVerticesAndTriangles( i2pt3idVertices, trivec );
}

int DatasetIsosurface::getEdgeID( unsigned int x, unsigned int y, unsigned int z, unsigned int nEdgeNo )
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

unsigned int DatasetIsosurface::getVertexID( unsigned int x, unsigned int y, unsigned int z )
{
    return 3 * ( z * ( y + 1 ) * ( x + 1 ) + y * ( x + 1 ) + x );
}

POINT3DID DatasetIsosurface::calculateIntersection( unsigned int x, unsigned int y, unsigned int z, unsigned int nEdgeNo )
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

    x1 = v1x * m_dX;
    y1 = v1y * m_dY;
    z1 = v1z * m_dZ;
    x2 = v2x * m_dX;
    y2 = v2y * m_dY;
    z2 = v2z * m_dZ;

    float val1 = m_scalarField[v1z * m_nPointsInSlice + v1y * m_nPointsInXDirection + v1x];
    float val2 = m_scalarField[v2z * m_nPointsInSlice + v2y * m_nPointsInXDirection + v2x];
    POINT3DID intersection = interpolate( x1, y1, z1, x2, y2, z2, val1, val2 );
    intersection.newID = 0;
    return intersection;
}

POINT3DID DatasetIsosurface::interpolate( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2 )
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

TriangleMesh2* DatasetIsosurface::renameVerticesAndTriangles( ID2POINT3DID &i2pt3idVertices, TRIANGLEVECTOR &trivec )
{
    unsigned int nextID = 0;

    TriangleMesh2* triMesh = new TriangleMesh2( i2pt3idVertices.size(), trivec.size() );

    float xOff = m_dX * 0.5f;
    float yOff = m_dY * 0.5f;
    float zOff = m_dZ * 0.5f;

    ID2POINT3DID::iterator mapIterator = i2pt3idVertices.begin();
    TRIANGLEVECTOR::iterator vecIterator = trivec.begin();

    // Rename vertices.
    while ( mapIterator != i2pt3idVertices.end() )
    {
        ( *mapIterator ).second.newID = nextID;
        triMesh->addVertex( nextID, ( *mapIterator ).second.x + xOff, ( *mapIterator ).second.y + yOff, ( *mapIterator ).second.z + zOff );
        ++nextID;
        ++mapIterator;
    }

    // Now rename triangles.
    int id = 0;
    while ( vecIterator != trivec.end() )
    {
        for ( unsigned int i = 0; i < 3; i++ )
        {
            unsigned int newID = i2pt3idVertices[( *vecIterator ).pointID[i]].newID;
            ( *vecIterator ).pointID[i] = newID;
        }
        triMesh->addTriangle( id, ( *vecIterator ).pointID[0], ( *vecIterator ).pointID[1], ( *vecIterator ).pointID[2] );
        ++vecIterator;
        ++id;
    }



    i2pt3idVertices.clear();
    trivec.clear();

    return triMesh;
}
