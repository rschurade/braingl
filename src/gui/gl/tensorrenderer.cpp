/*
 * shrenderer.cpp
 *
 *  Created on: 03.07.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include <limits>

#include <QtOpenGL/QGLShaderProgram>
#include <QtCore/QDebug>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../../data/datasets/datasettensor.h"
#include "../../data/enums.h"
#include "../../data/vptr.h"

#include "../../data/mesh/tesselation.h"
#include "../../data/mesh/trianglemesh.h"

#include "../../thirdparty/newmat10/newmat.h"

#include "glfunctions.h"
#include "tensorrenderer.h"

TensorRenderer::TensorRenderer() :
    ObjectRenderer(),
    vboIds( new GLuint[ 6 ] ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_xb( 0. ),
    m_yb( 0. ),
    m_zb( 0. ),
    m_xOld( -1 ),
    m_yOld( -1 ),
    m_zOld( -1 ),
    m_xbOld( -1 ),
    m_ybOld( -1 ),
    m_zbOld( -1 ),
    m_lodOld( -1 ),
    m_scalingOld( -1 ),
    m_renderSliceOld( 0 ),
    m_minmaxScalingOld( false ),
    m_lowerXOld( -1 ),
    m_lowerYOld( -1 ),
    m_lowerZOld( -1 ),
    m_upperXOld( -1 ),
    m_upperYOld( -1 ),
    m_upperZOld( -1 ),
    m_tris1( 0 )
{
    for ( int lod = 0; lod < 6; ++lod )
    {
        const Matrix* vertices = tess::vertices( lod );
        const int* faces = tess::faces( lod );
        int numVerts = tess::n_vertices( lod );
        int numTris = tess::n_faces( lod );

        TriangleMesh* mesh = new TriangleMesh( numVerts, numTris );

        for ( int i = 0; i < numVerts; ++i )
        {
            mesh->addVertex( i, QVector3D( (*vertices)( i+1, 1 ), (*vertices)( i+1, 2 ), (*vertices)( i+1, 3 ) ) );
        }
        for ( int i = 0; i < numTris; ++i )
        {
            Triangle tri = {faces[i*3], faces[i*3+1], faces[i*3+2]};
            mesh->addTriangle( i, tri );
        }
        m_spheres.push_back( mesh );
    }
}

TensorRenderer::~TensorRenderer()
{
}

void TensorRenderer::init()
{
    glGenBuffers( 6, vboIds );
}

void TensorRenderer::draw( QMatrix4x4 mvp_matrix )
{
    QList<int>rl;

    int countDatasets = model()->rowCount();
    int allocatedTextureCount = 0;
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model()->index( i, FNDSE_ACTIVE );
        if ( model()->data( index, Qt::EditRole ).toBool() )
        {
            index = model()->index( i, FNDSP_CREATED_BY );
            if ( model()->data( index, Qt::DisplayRole ).toInt() == FNALGO_TENSORFIT )
            {
                rl.push_back( i );
                //qDebug() << "found QBall to render";
            }
        }
    }

    if ( rl.size() > 0 )
    {
        //qDebug() << "sh draw";

        m_dataset = VPtr<DatasetTensor>::asPtr( model()->data( model()->index( rl[0], 2 ), Qt::EditRole ) );

        int renderOnSlice = m_dataset->getProperty( "renderSlice" ).toInt();
        if ( renderOnSlice == 0 )
        {
            return;
        }

        GLFunctions::getShader( "qball" )->bind();
        // Set modelview-projection matrix
        GLFunctions::getShader( "qball" )->setUniformValue( "mvp_matrix", mvp_matrix );


        initGeometry();

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        setShaderVars();
        glDrawElements( GL_TRIANGLES, m_tris1, GL_UNSIGNED_INT, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
}

void TensorRenderer::setupTextures()
{
}

void TensorRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "qball", model() );
}

void TensorRenderer::initGeometry()
{
    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();
    int xi = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toInt();
    int xbi = m_dataset->getProperty( "nx" ).toInt();
    int ybi = m_dataset->getProperty( "ny" ).toInt();
    int zbi = m_dataset->getProperty( "nz" ).toInt();

    float dx = m_dataset->getProperty( "dx" ).toFloat();
    float dy = m_dataset->getProperty( "dy" ).toFloat();
    float dz = m_dataset->getProperty( "dz" ).toFloat();

    int lowerX = m_dataset->getProperty( "renderLowerX" ).toInt();
    int lowerY = m_dataset->getProperty( "renderLowerY" ).toInt();
    int lowerZ = m_dataset->getProperty( "renderLowerZ" ).toInt();
    int upperX = m_dataset->getProperty( "renderUpperX" ).toInt();
    int upperY = m_dataset->getProperty( "renderUpperY" ).toInt();
    int upperZ = m_dataset->getProperty( "renderUpperZ" ).toInt();

    int order = m_dataset->getProperty( "order" ).toInt();

    int lod = m_dataset->getProperty( "lod" ).toInt();
    float scaling = m_dataset->getProperty( "scaling" ).toFloat();

    int renderOnSlice = m_dataset->getProperty( "renderSlice" ).toInt();

    bool minmaxScaling = m_dataset->getProperty( "minmaxScaling" ).toBool();
    bool minmaxScalingChanged = ( m_minmaxScalingOld != minmaxScaling );

    float x = m_x * dx + dx / 2.;
    float y = m_y * dy + dy / 2.;
    float z = m_z * dz + dz / 2.;

    TriangleMesh* mesh = m_spheres[lod];
    QVector<TriangleMesh*>balls;
    QVector< QVector3D > normals = mesh->getVertNormals();

    std::vector<float>verts;
    std::vector<int>indexes;

    bool datasetSizeChanged = ( xbi != m_xbOld || ybi != m_ybOld || zbi != m_zbOld );
    bool orientChanged = ( m_renderSliceOld != renderOnSlice );
    bool lodChanged = ( m_lodOld != lod );

    bool xSpanChanged = ( lowerX != m_lowerXOld || upperX != m_upperXOld );
    bool ySpanChanged = ( lowerY != m_lowerYOld || upperY != m_upperYOld );
    bool zSpanChanged = ( lowerZ != m_lowerZOld || upperZ != m_upperZOld );

    bool metaChanged = ( datasetSizeChanged || orientChanged || lodChanged || minmaxScalingChanged );

    bool xChanged = ( xi != m_xOld );
    bool yChanged = ( yi != m_yOld );
    bool zChanged = ( zi != m_zOld );

    QVector< QVector3D > vertices = mesh->getVertices();
    QVector< Triangle > triangles = mesh->getTriangles();

    QVector<Matrix>* data = m_dataset->getData();
    RowVector v1( 3 );
    ColumnVector v2( 3 );

    ColumnVector newVert( 3 );

    if ( renderOnSlice == 1 && ( metaChanged || zChanged || xSpanChanged || ySpanChanged ) )
    {
        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                Matrix D = data->at( xx + yy * xbi + zi * xbi * ybi );
                TriangleMesh* newBall = new TriangleMesh( vertices.size(), triangles.size() );

                for ( int i = 0; i < vertices.size(); ++i )
                {
                    v1( 1 ) = vertices[i].x();
                    v1( 2 ) = vertices[i].y();
                    v1( 3 ) = vertices[i].z();

                    v2( 1 ) = vertices[i].x();
                    v2( 2 ) = vertices[i].y();
                    v2( 3 ) = vertices[i].z();

                    float r = DotProduct( v1 * D, v2 );

                    newVert( 1 ) = r*vertices[i].x() + xx * dx + dx / 2;
                    newVert( 2 ) = r*vertices[i].y() + yy * dy + dy / 2;
                    newVert( 3 ) = r*vertices[i].z() + z;

                    newBall->addVertex( i, QVector3D( newVert( 1 ), newVert( 2 ), newVert( 3 ) ) );
                }

                for ( int i = 0; i < triangles.size(); ++i )
                {
                    newBall->addTriangle( i, Triangle({triangles[i].v0,triangles[i].v1,triangles[i].v2} ) );
                }

                balls.push_back( newBall );
            }
        }
    }
    else if ( renderOnSlice == 2 && ( metaChanged || yChanged || xSpanChanged || zSpanChanged ) )
    {
        for( int zz = lowerZ; zz < upperZ; ++zz )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                Matrix D = data->at( xx + yi * xbi + zz * xbi * ybi );
                TriangleMesh* newBall = new TriangleMesh( vertices.size(), triangles.size() );

                for ( int i = 0; i < vertices.size(); ++i )
                {
                    v1( 1 ) = vertices[i].x();
                    v1( 2 ) = vertices[i].y();
                    v1( 3 ) = vertices[i].z();

                    v2( 1 ) = vertices[i].x();
                    v2( 2 ) = vertices[i].y();
                    v2( 3 ) = vertices[i].z();

                    float r = DotProduct( v1 * D, v2 );

                    newVert( 1 ) = r*vertices[i].x() + xx * dx + dx / 2;
                    newVert( 2 ) = r*vertices[i].y() + y;
                    newVert( 3 ) = r*vertices[i].z() + zz * dz + dz / 2;

                    newBall->addVertex( i, QVector3D( newVert( 1 ), newVert( 2 ), newVert( 3 ) ) );
                }

                for ( int i = 0; i < triangles.size(); ++i )
                {
                    newBall->addTriangle( i, Triangle({triangles[i].v0,triangles[i].v1,triangles[i].v2} ) );
                }

                balls.push_back( newBall );
            }
        }
    }
    else if ( renderOnSlice == 3 && ( metaChanged || xChanged || ySpanChanged || zSpanChanged ) )
    {
        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int zz = lowerZ; zz < upperZ; ++zz )
            {
                Matrix D = data->at( xi + yy * xbi + zz * xbi * ybi );
                TriangleMesh* newBall = new TriangleMesh( vertices.size(), triangles.size() );

                for ( int i = 0; i < vertices.size(); ++i )
                {
                    v1( 1 ) = vertices[i].x();
                    v1( 2 ) = vertices[i].y();
                    v1( 3 ) = vertices[i].z();

                    v2( 1 ) = vertices[i].x();
                    v2( 2 ) = vertices[i].y();
                    v2( 3 ) = vertices[i].z();

                    float r = DotProduct( v1 * D, v2 );

                    newVert( 1 ) = r*vertices[i].x() + x;
                    newVert( 2 ) = r*vertices[i].y() + yy * dy + dy / 2;
                    newVert( 3 ) = r*vertices[i].z() + zz * dz + dz / 2;

                    newBall->addVertex( i, QVector3D( newVert( 1 ), newVert( 2 ), newVert( 3 ) ) );
                }

                for ( int i = 0; i < triangles.size(); ++i )
                {
                    newBall->addTriangle( i, Triangle({triangles[i].v0,triangles[i].v1,triangles[i].v2} ) );
                }

                balls.push_back( newBall );
            }
        }
    }
    else
    {
        return;
    }


    verts.reserve( balls.size() * mesh->getVertSize() * 6 );
    indexes.resize( balls.size() * mesh->getTriSize() * 3 );

    int numVerts = mesh->getVertSize();
    int currTri = 0;
    for ( int currBall = 0; currBall < balls.size(); ++currBall )
    {
        for ( int i = 0; i < balls[currBall]->getVertSize(); ++ i )
        {
            verts.push_back( balls[currBall]->getVertices()[i].x() );
            verts.push_back( balls[currBall]->getVertices()[i].y() );
            verts.push_back( balls[currBall]->getVertices()[i].z() );
//                verts.push_back( balls[currBall]->getVertNormals()[i].x() );
//                verts.push_back( balls[currBall]->getVertNormals()[i].y() );
//                verts.push_back( balls[currBall]->getVertNormals()[i].z() );
            verts.push_back( normals[i].x() );
            verts.push_back( normals[i].y() );
            verts.push_back( normals[i].z() );
        }
        for ( int i = 0; i < balls[currBall]->getTriSize(); ++ i )
        {
            indexes[currTri] = balls[currBall]->getTriangles()[i].v0 + currBall * numVerts;
            ++currTri;
            indexes[currTri] = balls[currBall]->getTriangles()[i].v1 + currBall * numVerts;
            ++currTri;
            indexes[currTri] = balls[currBall]->getTriangles()[i].v2 + currBall * numVerts;
            ++currTri;
        }

        delete balls[currBall];
    }

    m_tris1 = mesh->getTriSize() * balls.size() * 3;
    //qDebug() << m_tris1 << " " << verts.size() << " " << indexes.size();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );

    m_xOld = xi;
    m_yOld = yi;
    m_zOld = zi;
    m_xbOld = xbi;
    m_ybOld = ybi;
    m_zbOld = zbi;
    m_lodOld = lod;
    m_scalingOld = scaling;
    m_renderSliceOld = renderOnSlice;
    m_minmaxScalingOld = minmaxScaling;

    m_lowerXOld = lowerX;
    m_lowerYOld = lowerY;
    m_lowerZOld = lowerZ;
    m_upperXOld = upperX;
    m_upperYOld = upperY;
    m_upperZOld = upperZ;
}
