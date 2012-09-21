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
    m_tris1( 0 ),
    vboIds( new GLuint[ 6 ] )
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

        int orient = m_dataset->getProperty( "renderSlice" ).toInt();
        if ( orient == 0 )
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
    int xi = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toInt();
    int nx = m_dataset->getProperty( "nx" ).toInt();
    int ny = m_dataset->getProperty( "ny" ).toInt();
    int nz = m_dataset->getProperty( "nz" ).toInt();
    float dx = m_dataset->getProperty( "dx" ).toFloat();
    float dy = m_dataset->getProperty( "dy" ).toFloat();
    float dz = m_dataset->getProperty( "dz" ).toFloat();

    int orient = m_dataset->getProperty( "renderSlice" ).toInt();

    calcBounds( nx, ny, nz, dx, dy, dz, orient );

    int lowerX = m_visibleArea[0];
    int upperX = m_visibleArea[1];
    int lowerY = m_visibleArea[2];
    int upperY = m_visibleArea[3];
    int lowerZ = m_visibleArea[4];
    int upperZ = m_visibleArea[5];

    int bValue = m_dataset->getProperty( "bValue" ).toInt();

    //float scaling = m_dataset->getProperty( "scaling" ).toFloat();


    bool minmaxScaling = m_dataset->getProperty( "minmaxScaling" ).toBool();

    QString s = createSettingsString( xi, yi, zi, orient, lowerX, upperX, lowerY, upperY, lowerZ, upperZ, minmaxScaling, bValue);
    if ( s == m_previousSettings || orient == FN_NONE )
    {
        return;
    }
    m_previousSettings = s;

    int lod = getMaxLod( orient, lowerX, upperX, lowerY, upperY, lowerZ, upperZ );
    qDebug() << "Tensor Renderer: using lod " << lod;


    TriangleMesh* mesh = m_spheres[lod];
    QVector< QVector3D > normals = mesh->getVertNormals();

    std::vector<float>verts;
    std::vector<int>indexes;

    QVector< QVector3D > vertices = mesh->getVertices();
    QVector< Triangle > triangles = mesh->getTriangles();

    QVector<Matrix>* data = m_dataset->getData();
    RowVector v1( 3 );
    ColumnVector v2( 3 );

    ColumnVector newVert( 3 );

    float x = (float)xi * dx + dx / 2.;
    float y = (float)yi * dy + dy / 2.;
    float z = (float)zi * dz + dz / 2.;

    int numVerts = mesh->getVertSize();
    int currentBall = 0;

    if ( orient == 1 )
    {
        //qDebug() << "Tensor Renderer: start init geometry";

        int glyphs = ( upperX - lowerX ) * ( upperY - lowerY );
        verts.reserve( mesh->getVertSize() * glyphs * 10 );
        indexes.reserve( triangles.size() * glyphs * 3 );

        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                Matrix D = data->at( xx + yy * nx + zi * nx * ny );

                for ( int i = 0; i < vertices.size(); ++i )
                {
                    v1( 1 ) = vertices[i].x();
                    v1( 2 ) = vertices[i].y();
                    v1( 3 ) = vertices[i].z();

                    v2( 1 ) = vertices[i].x();
                    v2( 2 ) = vertices[i].y();
                    v2( 3 ) = vertices[i].z();

                    float r = DotProduct( v1 * D, v2 ) * bValue;

                    float locX = xx * dx + dx / 2;
                    float locY =  + yy * dy + dy / 2;

                    verts.push_back( vertices[i].x() );
                    verts.push_back( vertices[i].y() );
                    verts.push_back( vertices[i].z() );
                    verts.push_back( normals[i].x() );
                    verts.push_back( normals[i].y() );
                    verts.push_back( normals[i].z() );
                    verts.push_back( locX );
                    verts.push_back( locY );
                    verts.push_back( z );
                    verts.push_back( r );
                }
                for ( int i = 0; i < triangles.size(); ++i )
                {
                    indexes.push_back( triangles[i].v0 + numVerts * currentBall );
                    indexes.push_back( triangles[i].v1 + numVerts * currentBall );
                    indexes.push_back( triangles[i].v2 + numVerts * currentBall );
                }
                ++currentBall;
            }
        }
    }
    else if ( orient == 2 )
    {
        //qDebug() << "Tensor Renderer: start init geometry";
        for( int zz = lowerZ; zz < upperZ; ++zz )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                Matrix D = data->at( xx + yi * nx + zz * nx * ny );

                for ( int i = 0; i < vertices.size(); ++i )
                {
                    v1( 1 ) = vertices[i].x();
                    v1( 2 ) = vertices[i].y();
                    v1( 3 ) = vertices[i].z();

                    v2( 1 ) = vertices[i].x();
                    v2( 2 ) = vertices[i].y();
                    v2( 3 ) = vertices[i].z();

                    float r = DotProduct( v1 * D, v2 ) * bValue;

                    float locX = xx * dx + dx / 2;
                    float locZ = zz * dz + dz / 2;

                    verts.push_back( vertices[i].x() );
                    verts.push_back( vertices[i].y() );
                    verts.push_back( vertices[i].z() );
                    verts.push_back( normals[i].x() );
                    verts.push_back( normals[i].y() );
                    verts.push_back( normals[i].z() );
                    verts.push_back( locX );
                    verts.push_back( y );
                    verts.push_back( locZ );
                    verts.push_back( r );
                }
                for ( int i = 0; i < triangles.size(); ++i )
                {
                    indexes.push_back( triangles[i].v0 + numVerts * currentBall );
                    indexes.push_back( triangles[i].v1 + numVerts * currentBall );
                    indexes.push_back( triangles[i].v2 + numVerts * currentBall );
                }
                ++currentBall;
            }
        }
    }
    else if ( orient == 3 )
    {
        //qDebug() << "Tensor Renderer: start init geometry";
        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int zz = lowerZ; zz < upperZ; ++zz )
            {
                Matrix D = data->at( xi + yy * nx + zz * nx * ny );

                for ( int i = 0; i < vertices.size(); ++i )
                {
                    v1( 1 ) = vertices[i].x();
                    v1( 2 ) = vertices[i].y();
                    v1( 3 ) = vertices[i].z();

                    v2( 1 ) = vertices[i].x();
                    v2( 2 ) = vertices[i].y();
                    v2( 3 ) = vertices[i].z();

                    float r = DotProduct( v1 * D, v2 ) * bValue;

                    float locY = yy * dy + dy / 2;
                    float locZ = zz * dz + dz / 2;

                    verts.push_back( vertices[i].x() );
                    verts.push_back( vertices[i].y() );
                    verts.push_back( vertices[i].z() );
                    verts.push_back( normals[i].x() );
                    verts.push_back( normals[i].y() );
                    verts.push_back( normals[i].z() );
                    verts.push_back( x );
                    verts.push_back( locY );
                    verts.push_back( locZ );
                    verts.push_back( r );
                }
                for ( int i = 0; i < triangles.size(); ++i )
                {
                    indexes.push_back( triangles[i].v0 + numVerts * currentBall );
                    indexes.push_back( triangles[i].v1 + numVerts * currentBall );
                    indexes.push_back( triangles[i].v2 + numVerts * currentBall );
                }
                ++currentBall;
            }
        }
    }
    else
    {
        return;
    }

    m_tris1 = mesh->getTriSize() * currentBall * 3;

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );

    //qDebug() << "Tensor Renderer: end init geometry";
}
