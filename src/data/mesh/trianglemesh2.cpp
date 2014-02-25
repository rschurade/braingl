/*
 * trianglemesh2.cpp
 *
 * Created on: Dec 4, 2012
 * @author Ralph Schurade
 */
#include "trianglemesh2.h"
#include "meshthread.h"
#include "octree.h"

#include "../../gui/gl/glfunctions.h"

#include <QDebug>

TriangleMesh2::TriangleMesh2( unsigned int numVerts, unsigned int numTris ) :
    m_bufferSize( 7 ),
    m_numVerts( numVerts ),
    m_numTris( numTris ),
    m_vertexInsertId( 0 ),
    m_colorInsertId( 0 ),
    m_triangleInsertId( 0 ),
    m_ocTree( 0 )
{
    m_vertices.resize( numVerts * m_bufferSize );
    m_vertexColors.resize( numVerts * 4 );
    m_vertIsInTriangle.resize( numVerts );
    m_vertNeighbors.resize( numVerts );

    m_triangles.resize( numTris * 3 );
    m_triNormals.resize( numTris );

    m_numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new MeshThread( &m_vertices, &m_triangles, m_numTris, m_bufferSize, i ) );
    }
}

TriangleMesh2::TriangleMesh2( TriangleMesh2* trim ) :
    m_bufferSize( 7 ),
    m_numVerts( trim->numVerts() ),
    m_numTris( trim->numTris() ),
    m_vertexInsertId( 0 ),
    m_colorInsertId( 0 ),
    m_triangleInsertId( 0 ),
    m_ocTree( 0 )
{
    m_vertices.resize( trim->numVerts() * m_bufferSize );
    m_vertexColors.resize( trim->numVerts() * 4 );
    m_vertIsInTriangle.resize( trim->numVerts() );
    m_vertNeighbors.resize( trim->numVerts() );

    m_triangles.resize( trim->numTris() * 3 );
    m_triNormals.resize( trim->numTris() );

    m_numThreads = GLFunctions::idealThreadCount;

    m_triangleInsertId = m_numTris * 3;
    m_vertexInsertId = m_numVerts * m_bufferSize;
    m_colorInsertId = m_numVerts * 4;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new MeshThread( &m_vertices, &m_triangles, m_numTris, m_bufferSize, i ) );
    }

    for ( unsigned int i = 0; i < trim->numVerts(); ++i )
    {
        QVector3D v = trim->getVertex( i );
        this->setVertex( i, v.x(), v.y(), v.z() );
    }
    std::vector<unsigned int> tris = trim->getTriangles();
    for ( unsigned int i = 0; i < trim->numTris(); ++i )
    {
        this->setTriangle( i, tris[i * 3], tris[i * 3 + 1], tris[i * 3 + 2] );
    }
    finalize();
}

TriangleMesh2::~TriangleMesh2()
{
    m_vertices.clear();
    m_vertexColors.clear();
    m_vertIsInTriangle.clear();
    m_vertNeighbors.clear();
    m_triangles.clear();
    m_triNormals.clear();
    m_threads.clear();
    m_toRemove.clear();
}

void TriangleMesh2::resize( unsigned int numVerts, unsigned int numTris )
{
    m_vertexInsertId = m_numVerts * bufferSize();
    m_triangleInsertId = m_numTris * 3;
    m_colorInsertId = m_numVerts * 4;

    m_vertices.resize( numVerts * m_bufferSize );
    m_vertexColors.resize( numVerts * 4 );
    m_vertIsInTriangle.resize( numVerts );
    m_vertNeighbors.resize( numVerts );

    m_numVerts = numVerts;

    m_triangles.resize( numTris * 3 );
    m_triNormals.resize( numTris );

    m_numTris = numTris;

    // create threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        delete m_threads[i];
    }
    m_threads.clear();
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads.push_back( new MeshThread( &m_vertices, &m_triangles, m_numTris, m_bufferSize, i ) );
    }
}

unsigned int TriangleMesh2::bufferSize()
{
    return m_bufferSize;
}

void TriangleMesh2::finalize()
{
//    if ( m_ocTree == 0 )
//    {
//        buildOcTree();
//    }

    calcTriNormals();
    calcVertNormals();
    //qDebug() << m_numVerts << "vertices";
}

void TriangleMesh2::setVertex( unsigned int id, float x, float y, float z )
{
    m_vertices[ id * m_bufferSize     ] = x;
    m_vertices[ id * m_bufferSize + 1 ] = y;
    m_vertices[ id * m_bufferSize + 2 ] = z;

    m_vertexColors[ id * 4     ] = 1.0;
    m_vertexColors[ id * 4 + 1 ] = 1.0;
    m_vertexColors[ id * 4 + 2 ] = 1.0;
    m_vertexColors[ id * 4 + 3 ] = 1.0;
}

void TriangleMesh2::setVertex( unsigned int id, QVector3D pos )
{
    setVertex( id, pos.x(), pos.y(), pos.z() );
}

bool TriangleMesh2::addVertex( float x, float y, float z )
{
    if(  m_vertices.size() > m_vertexInsertId )
    {
        m_vertices[ m_vertexInsertId++ ] = x;
        m_vertices[ m_vertexInsertId++ ] = y;
        m_vertices[ m_vertexInsertId++ ] = z;

        m_vertexInsertId += 4;

        m_vertexColors[ m_colorInsertId++ ] = 1.0;
        m_vertexColors[ m_colorInsertId++ ] = 1.0;
        m_vertexColors[ m_colorInsertId++ ] = 1.0;
        m_vertexColors[ m_colorInsertId++ ] = 1.0;
        return true;
    }
    else
    {
        //qDebug() << "adding vertex beyond field size";
        m_vertices.push_back( x );
        m_vertices.push_back( y );
        m_vertices.push_back( z );
        m_vertices.push_back( 1.0 );
        m_vertices.push_back( 1.0 );
        m_vertices.push_back( 1.0 );
        m_vertices.push_back( 1.0 );

        m_vertexColors.push_back( 1.0 );
        m_vertexColors.push_back( 1.0 );
        m_vertexColors.push_back( 1.0 );
        m_vertexColors.push_back( 1.0 );

        m_vertexInsertId += 7;
        m_colorInsertId += 4;

        std::vector<unsigned int>v;
        m_vertIsInTriangle.push_back( v );
        m_vertNeighbors.push_back( v );

        ++m_numVerts;
        return false;
    }
}

QVector3D TriangleMesh2::getVertex( unsigned int id )
{
    if ( id > m_numVerts )
    {
        qDebug() << "***error*** index out of range, numVerts =" << m_numVerts << "id:" << id;
        exit( 0 );
    }
    QVector3D vert( m_vertices[id * m_bufferSize], m_vertices[id * m_bufferSize + 1], m_vertices[id * m_bufferSize + 2]);
    return vert;
}

QVector3D TriangleMesh2::getVertexNormal( unsigned int id )
{
    if ( id > m_numVerts )
    {
        qDebug() << "***error*** index out of range, numVerts =" << m_numVerts << "id:" << id;
        exit( 0 );
    }
    QVector3D normal( m_vertices[id * m_bufferSize + 3], m_vertices[id * m_bufferSize + 4], m_vertices[id * m_bufferSize + 5]);
    return normal;
}

void TriangleMesh2::setTriangle( unsigned int id, unsigned int v0, unsigned int v1, unsigned int v2 )
{
    m_triangles[ id * 3     ] = v0;
    m_triangles[ id * 3 + 1 ] = v1;
    m_triangles[ id * 3 + 2 ] = v2;

    m_vertIsInTriangle[v0].push_back( id );
    m_vertIsInTriangle[v1].push_back( id );
    m_vertIsInTriangle[v2].push_back( id );
}

void TriangleMesh2::setTriangle( unsigned int id, Triangle tri )
{
    m_triangles[ id * 3     ] = tri.v0;
    m_triangles[ id * 3 + 1 ] = tri.v1;
    m_triangles[ id * 3 + 2 ] = tri.v2;

    m_vertIsInTriangle[tri.v0].push_back( id );
    m_vertIsInTriangle[tri.v1].push_back( id );
    m_vertIsInTriangle[tri.v2].push_back( id );
}

void TriangleMesh2::addTriangle( unsigned int v0, unsigned int v1, unsigned int v2 )
{
    int id = m_triangleInsertId / 3;
    m_vertIsInTriangle[v0].push_back( id );
    m_vertIsInTriangle[v1].push_back( id );
    m_vertIsInTriangle[v2].push_back( id );

    m_triangles[ m_triangleInsertId++ ] = v0;
    m_triangles[ m_triangleInsertId++ ] = v1;
    m_triangles[ m_triangleInsertId++ ] = v2;
}

void TriangleMesh2::addTriangle( Triangle tri )
{
    int id = m_triangleInsertId / 3;
    m_vertIsInTriangle[tri.v0].push_back( id );
    m_vertIsInTriangle[tri.v1].push_back( id );
    m_vertIsInTriangle[tri.v2].push_back( id );

    m_triangles[ m_triangleInsertId++ ] = tri.v0;
    m_triangles[ m_triangleInsertId++ ] = tri.v1;
    m_triangles[ m_triangleInsertId++ ] = tri.v2;
}


void TriangleMesh2::setVertexColor( unsigned int id, QColor color )
{
    setVertexColor( id, color.redF(), color.greenF(), color.blueF(), color.alphaF() );
}

void TriangleMesh2::setVertexColor( unsigned int id, float r, float g, float b, float a )
{
    m_vertexColors[ id * 4     ] = r;
    m_vertexColors[ id * 4 + 1 ] = g;
    m_vertexColors[ id * 4 + 2 ] = b;
    m_vertexColors[ id * 4 + 3 ] = a;
}

QColor TriangleMesh2::getVertexColor( unsigned int id )
{
    float r = m_vertexColors[ id * 4     ];
    float g = m_vertexColors[ id * 4 + 1 ];
    float b = m_vertexColors[ id * 4 + 2 ];
    float a = m_vertexColors[ id * 4 + 3 ];
    return QColor::fromRgbF(r,g,b,a);
}

void TriangleMesh2::setVertexData( unsigned int id, float value )
{
    m_vertices[ id * m_bufferSize + 6 ] = value;
}

float TriangleMesh2::getVertexData( unsigned int id )
{
    return m_vertices[ id * m_bufferSize + 6 ];
}

float* TriangleMesh2::getVertices()
{
    return m_vertices.data();
}

float* TriangleMesh2::getVertexColors()
{
    return m_vertexColors.data();
}

unsigned int* TriangleMesh2::getIndexes()
{
    return m_triangles.data();
}

std::vector<unsigned int> TriangleMesh2::getTriangle( unsigned int id )
{
    if ( id > m_numTris )
    {
        qDebug() << "***error*** index out of range, numTris =" << m_numTris << "id:" << id;
        exit( 0 );
    }
    std::vector<unsigned int> out;
    out.push_back( m_triangles[id * 3] );
    out.push_back( m_triangles[id * 3 + 1] );
    out.push_back( m_triangles[id * 3 + 2] );
    return out;
}

Triangle TriangleMesh2::getTriangle2( unsigned int id )
{
    if ( id > m_numTris )
    {
        qDebug() << "***error*** index out of range, numTris =" << m_numTris << "id:" << id;
        exit( 0 );
    }
    Triangle out;
    out.v0 = m_triangles[id * 3];
    out.v1 = m_triangles[id * 3 + 1];
    out.v2 = m_triangles[id * 3 + 2];
    return out;
}


std::vector<unsigned int> TriangleMesh2::getStar( unsigned int id )
{
    return m_vertIsInTriangle[id];
}

unsigned int TriangleMesh2::getNextVertex( unsigned int triNum, unsigned int vertNum )
{
    int answer = -1;

    if ( m_triangles[triNum * 3] == vertNum )
    {
        answer = m_triangles[triNum * 3 + 1];
    }
    else if ( m_triangles[triNum * 3 + 1] == vertNum )
    {
        answer = m_triangles[triNum * 3 + 2];
    }
    else
    {
        answer = m_triangles[triNum * 3];
    }

    return answer;
}

unsigned int TriangleMesh2::getThirdVert( unsigned int coVert1, unsigned int coVert2, unsigned int triangleNum )
{
    int index = 0;
    bool found = false;

    while ( ( index < 2 ) && !found )
    {
        if ( ( m_triangles[triangleNum * 3 + index] == coVert1 ) || ( m_triangles[triangleNum * 3 + index] == coVert2 ) )
        {
            ++index;
        }
        else
        {
            found = true;
        }
    }

    return m_triangles[triangleNum * 3 + index];
}

void TriangleMesh2::calcTriNormals()
{
    // run threads
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_threads[i]->wait();
    }
}

void TriangleMesh2::calcVertNormals()
{
    m_triNormals.clear();
    for ( int i = 0; i < m_numThreads; ++i )
    {
        m_triNormals.insert( m_triNormals.end(), m_threads[i]->getTriNormals()->begin(), m_threads[i]->getTriNormals()->end() );
    }

    for ( unsigned int i = 0; i < m_numVerts; ++i )
    {
        QVector3D sum( 0, 0, 0 );
        for ( unsigned int k = 0; k < m_vertIsInTriangle[i].size(); ++k )
        {
            sum += m_triNormals[m_vertIsInTriangle[i][k]];
        }
        sum.normalize();
        m_vertices[ m_bufferSize * i + 3 ] = sum.x();
        m_vertices[ m_bufferSize * i + 4 ] = sum.y();
        m_vertices[ m_bufferSize * i + 5 ] = sum.z();
    }
}

void TriangleMesh2::buildOcTree()
{
    qDebug() << "start building OcTree";

    m_ocTree = new OcTree( QVector3D( 0, 0, 0), 512 );
    for( unsigned int i = 0; i < m_numVerts; ++i )
    {
        m_ocTree->insert( i, QVector3D( m_vertices[ m_bufferSize * i], m_vertices[ m_bufferSize * i + 1], m_vertices[ m_bufferSize * i + 2] ) );
        /*
        int id = m_ocTree->insert( i, QVector3D( m_vertices[ m_bufferSize * i], m_vertices[ m_bufferSize * i + 1], m_vertices[ m_bufferSize * i + 2] ) );

        if ( id != -1 )
        {
            collapseVertex( id, i );
        }
        */
    }
    //qDebug() << "collapsed" << m_toRemove.size() << "vertexes";
    qDebug() << "end building OcTree";
}

void TriangleMesh2::collapseVertex( unsigned int toId, unsigned int toRemoveId )
{
    m_toRemove.push_back( toRemoveId );
    for( unsigned int i = 0; i < m_triangles.size(); ++i )
    {
        if( m_triangles[i] == toRemoveId )
        {
            m_triangles[i] = toId;
        }
    }
}

std::vector<unsigned int> TriangleMesh2::pick( QVector3D pos, float radius )
{
    std::vector<unsigned int> result;

    for ( unsigned int i = 0; i < m_numVerts; ++i )
    {
        float d = ( QVector3D( m_vertices[i*m_bufferSize], m_vertices[i*m_bufferSize+1], m_vertices[i*m_bufferSize+2]) - pos ).lengthSquared();
        if ( d < radius )
        {
            result.push_back( i );
        }
    }

    return result;
}

unsigned int TriangleMesh2::closestVertexIndex( QVector3D pos )
{
    int closest = -1;
    double minRadius = 10000000;
    for ( unsigned int i = 0; i < m_numVerts; ++i )
    {
        float d = ( QVector3D( m_vertices[i * m_bufferSize], m_vertices[i * m_bufferSize + 1], m_vertices[i * m_bufferSize + 2] ) - pos ).lengthSquared();
        if ( d < minRadius )
        {
            minRadius = d;
            closest = i;
        }
    }
    return closest;
}

unsigned int TriangleMesh2::getNeighbor( unsigned int coVert1, unsigned int coVert2, unsigned int triangleNum )
{
    std::vector<unsigned int>candidates = m_vertIsInTriangle[coVert1];
    std::vector<unsigned int>compares   = m_vertIsInTriangle[coVert2];

    for ( unsigned int i = 0; i < candidates.size(); ++i )
    {
        for ( unsigned int k = 0; k < compares.size(); ++k )
        {
            if ( ( candidates[i] != triangleNum ) && ( candidates[i] == compares[k] ) )
            {
                return candidates[i];
            }
        }
    }
    return triangleNum;
}
