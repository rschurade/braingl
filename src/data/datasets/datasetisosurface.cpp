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

    m_isoLevel = getProperty( "isoValue").toFloat();
    if ( m_oldIsoValue != m_isoLevel )
    {
        delete m_mesh;
        generateSurface();
        m_oldIsoValue = m_isoLevel;
        m_renderer->setMesh( m_mesh );
    }

    m_renderer->draw( mvpMatrix, mvMatrixInverse );
}

QString DatasetIsosurface::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}

void DatasetIsosurface::generateSurface()
{
    m_nX = getProperty( "nx" ).toInt() - 1;
    m_nY = getProperty( "ny" ).toInt() - 1;
    m_nZ = getProperty( "nz" ).toInt() - 1;

    m_dX = getProperty( "dx" ).toFloat();
    m_dY = getProperty( "dy" ).toFloat();
    m_dZ = getProperty( "dz" ).toFloat();


    unsigned int nPointsInXDirection = (m_nX + 1);
    unsigned int nPointsInSlice = nPointsInXDirection * (m_nY + 1);

    // Generate isosurface.
    for (unsigned int z = 0; z < m_nZ; ++z)
    {
        for (unsigned int y = 0; y < m_nY; ++y)
        {
            for (unsigned int x = 0; x < m_nX; ++x)
            {
                // Calculate table lookup index from those
                // vertices which are below the isolevel.
                unsigned int tableIndex = 0;
                if (m_scalarField[z * nPointsInSlice + y * nPointsInXDirection + x] < m_isoLevel)
                    tableIndex |= 1;
                if (m_scalarField[z * nPointsInSlice + (y + 1) * nPointsInXDirection + x] < m_isoLevel)
                    tableIndex |= 2;
                if (m_scalarField[z * nPointsInSlice + (y + 1) * nPointsInXDirection + (x + 1)]
                        < m_isoLevel)
                    tableIndex |= 4;
                if (m_scalarField[z * nPointsInSlice + y * nPointsInXDirection + (x + 1)] < m_isoLevel)
                    tableIndex |= 8;
                if (m_scalarField[(z + 1) * nPointsInSlice + y * nPointsInXDirection + x] < m_isoLevel)
                    tableIndex |= 16;
                if (m_scalarField[(z + 1) * nPointsInSlice + (y + 1) * nPointsInXDirection + x]
                        < m_isoLevel)
                    tableIndex |= 32;
                if (m_scalarField[(z + 1) * nPointsInSlice + (y + 1) * nPointsInXDirection + (x + 1)]
                        < m_isoLevel)
                    tableIndex |= 64;
                if (m_scalarField[(z + 1) * nPointsInSlice + y * nPointsInXDirection + (x + 1)]
                        < m_isoLevel)
                    tableIndex |= 128;

                // Now create a triangulation of the isosurface in this
                // cell.
                if (edgeTable[tableIndex] != 0)
                {
                    if (edgeTable[tableIndex] & 8)
                    {
                        POINT3DID pt = calculateIntersection(x, y, z, 3);
                        unsigned int id = getEdgeID(x, y, z, 3);
                        m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                    }
                    if (edgeTable[tableIndex] & 1)
                    {
                        POINT3DID pt = calculateIntersection(x, y, z, 0);
                        unsigned int id = getEdgeID(x, y, z, 0);
                        m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                    }
                    if (edgeTable[tableIndex] & 256)
                    {
                        POINT3DID pt = calculateIntersection(x, y, z, 8);
                        unsigned int id = getEdgeID(x, y, z, 8);
                        m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                    }

                    if (x == m_nX - 1)
                    {
                        if (edgeTable[tableIndex] & 4)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 2);
                            unsigned int id = getEdgeID(x, y, z, 2);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                        if (edgeTable[tableIndex] & 2048)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 11);
                            unsigned int id = getEdgeID(x, y, z, 11);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                    }
                    if (y == m_nY - 1)
                    {
                        if (edgeTable[tableIndex] & 2)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 1);
                            unsigned int id = getEdgeID(x, y, z, 1);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                        if (edgeTable[tableIndex] & 512)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 9);
                            unsigned int id = getEdgeID(x, y, z, 9);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                    }
                    if (z == m_nZ - 1)
                    {
                        if (edgeTable[tableIndex] & 16)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 4);
                            unsigned int id = getEdgeID(x, y, z, 4);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                        if (edgeTable[tableIndex] & 128)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 7);
                            unsigned int id = getEdgeID(x, y, z, 7);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                    }
                    if ((x == m_nX - 1) && (y == m_nY - 1))
                        if (edgeTable[tableIndex] & 1024)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 10);
                            unsigned int id = getEdgeID(x, y, z, 10);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                    if ((x == m_nX - 1) && (z == m_nZ - 1))
                        if (edgeTable[tableIndex] & 64)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 6);
                            unsigned int id = getEdgeID(x, y, z, 6);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }
                    if ((y == m_nY - 1) && (z == m_nZ - 1))
                        if (edgeTable[tableIndex] & 32)
                        {
                            POINT3DID pt = calculateIntersection(x, y, z, 5);
                            unsigned int id = getEdgeID(x, y, z, 5);
                            m_i2pt3idVertices.insert(ID2POINT3DID::value_type(id, pt));
                        }

                    for (int i = 0; triTable[tableIndex][i] != -1; i += 3)
                    {
                        TRIANGLE triangle;
                        unsigned int pointID0, pointID1, pointID2;
                        pointID0 = getEdgeID(x, y, z, triTable[tableIndex][i]);
                        pointID1 = getEdgeID(x, y, z, triTable[tableIndex][i + 1]);
                        pointID2 = getEdgeID(x, y, z, triTable[tableIndex][i + 2]);
                        triangle.pointID[0] = pointID0;
                        triangle.pointID[1] = pointID1;
                        triangle.pointID[2] = pointID2;
                        m_trivecTriangles.push_back(triangle);
                    }
                }
            }
        }
    }

    renameVerticesAndTriangles();

}

int DatasetIsosurface::getEdgeID(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo)
{
    switch (nEdgeNo)
    {
        case 0:
            return getVertexID(nX, nY, nZ) + 1;
        case 1:
            return getVertexID(nX, nY + 1, nZ);
        case 2:
            return getVertexID(nX + 1, nY, nZ) + 1;
        case 3:
            return getVertexID(nX, nY, nZ);
        case 4:
            return getVertexID(nX, nY, nZ + 1) + 1;
        case 5:
            return getVertexID(nX, nY + 1, nZ + 1);
        case 6:
            return getVertexID(nX + 1, nY, nZ + 1) + 1;
        case 7:
            return getVertexID(nX, nY, nZ + 1);
        case 8:
            return getVertexID(nX, nY, nZ) + 2;
        case 9:
            return getVertexID(nX, nY + 1, nZ) + 2;
        case 10:
            return getVertexID(nX + 1, nY + 1, nZ) + 2;
        case 11:
            return getVertexID(nX + 1, nY, nZ) + 2;
        default:
            // Invalid edge no.
            return -1;
    }
}

unsigned int DatasetIsosurface::getVertexID(unsigned int nX, unsigned int nY, unsigned int nZ)
{
    return 3* (nZ *(m_nY + 1)*(m_nX + 1) + nY*(m_nX + 1) + nX);
}

POINT3DID DatasetIsosurface::calculateIntersection(unsigned int nX, unsigned int nY, unsigned int nZ,
        unsigned int nEdgeNo)
{
    float x1, y1, z1, x2, y2, z2;
    unsigned int v1x = nX, v1y = nY, v1z = nZ;
    unsigned int v2x = nX, v2y = nY, v2z = nZ;

    switch (nEdgeNo)
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

    unsigned int nPointsInXDirection = (m_nX + 1);
    unsigned int nPointsInSlice = nPointsInXDirection * (m_nY + 1);
    float val1 = m_scalarField[v1z * nPointsInSlice + v1y * nPointsInXDirection + v1x];
    float val2 = m_scalarField[v2z * nPointsInSlice + v2y * nPointsInXDirection + v2x];
    POINT3DID intersection = interpolate(x1, y1, z1, x2, y2, z2, val1, val2);
    intersection.newID = 0;
    return intersection;
}

POINT3DID DatasetIsosurface::interpolate(float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float tVal1, float tVal2)
{
    POINT3DID interpolation;
    float mu;

    mu = float((m_isoLevel - tVal1)) / (tVal2 - tVal1);
    interpolation.x = fX1 + mu * (fX2 - fX1);
    interpolation.y = fY1 + mu * (fY2 - fY1);
    interpolation.z = fZ1 + mu * (fZ2 - fZ1);
    interpolation.newID = 0;
    return interpolation;
}

void DatasetIsosurface::renameVerticesAndTriangles()
{
    unsigned int nextID = 0;
    ID2POINT3DID::iterator mapIterator = m_i2pt3idVertices.begin();
    TRIANGLEVECTOR::iterator vecIterator = m_trivecTriangles.begin();

    m_mesh = new TriangleMesh2( m_i2pt3idVertices.size(), m_trivecTriangles.size() );

    float xOff = 0.5f;
    float yOff = 0.5f;
    float zOff = 0.5f;

    // Rename vertices.
    while (mapIterator != m_i2pt3idVertices.end())
    {
        (*mapIterator).second.newID = nextID;
        m_mesh->addVertex( nextID, (*mapIterator).second.x + xOff, (*mapIterator).second.y + yOff, (*mapIterator).second.z + zOff);
        ++nextID;
        ++mapIterator;
    }

    // Now rename triangles.
    int id = 0;
    while (vecIterator != m_trivecTriangles.end())
    {
        for (unsigned int i = 0; i < 3; i++)
        {
            unsigned int newID = m_i2pt3idVertices[(*vecIterator).pointID[i]].newID;
            (*vecIterator).pointID[i] = newID;
        }
        m_mesh->addTriangle(id, (*vecIterator).pointID[0], (*vecIterator).pointID[1], (*vecIterator).pointID[2]);
        ++vecIterator;
        ++id;
    }
    m_mesh->finalize();

    m_i2pt3idVertices.clear();
    m_trivecTriangles.clear();
}
