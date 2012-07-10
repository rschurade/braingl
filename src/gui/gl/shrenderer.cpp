/*
 * shrenderer.cpp
 *
 *  Created on: 03.07.2012
 *      Author: Ralph
 */
#include "../../thirdparty/glew/include/glew.h"

#include <limits>
#include <omp.h>

#include <QtOpenGL/QGLShaderProgram>
#include <QtCore/QDebug>
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../../data/datasets/datasetdwi.h"
#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/qball.h"

#include "../../data/mesh/tesselation.h"
#include "../../data/mesh/trianglemesh.h"

#include "../../thirdparty/newmat10/newmat.h"

#include "glfunctions.h"
#include "shrenderer.h"

SHRenderer::SHRenderer() :
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

        const Matrix* v1 = tess::vertices( lod );
        Matrix base = ( QBall::sh_base( (*v1), 4 ) );
        m_bases.push_back( base );
    }
}

SHRenderer::~SHRenderer()
{
}

void SHRenderer::init()
{
    glGenBuffers( 6, vboIds );
}

void SHRenderer::draw( QMatrix4x4 mvp_matrix )
{
    QList<int>rl;

    int countDatasets = model()->rowCount();
    int allocatedTextureCount = 0;
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model()->index( i, 55 );
        if ( model()->data( index, Qt::EditRole ).toBool() )
        {
            index = model()->index( i, 13 );
            if ( model()->data( index, Qt::DisplayRole ).toInt() == FNALGO_QBALL )
            {
                rl.push_back( i );
                //qDebug() << "found QBall to render";
            }
        }
    }

    if ( rl.size() > 0 )
    {
        //qDebug() << "sh draw";

        m_dataset = VPtr<DatasetDWI>::asPtr( model()->data( model()->index( rl[0], 2 ), Qt::EditRole ) );

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

void SHRenderer::setupTextures()
{
}

void SHRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "qball", model() );
}

void SHRenderer::initGeometry()
{
    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();
    int xi = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toInt();
    int xbi = model()->data( model()->index( 0, 103 ), Qt::UserRole ).toInt();
    int ybi = model()->data( model()->index( 0, 104 ), Qt::UserRole ).toInt();
    int zbi = model()->data( model()->index( 0, 105 ), Qt::UserRole ).toInt();

    float dx = model()->data( model()->index( 0, 106 ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, 107 ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, 108 ), Qt::UserRole ).toFloat();

    int lod = m_dataset->getProperty( "lod" ).toInt();
    float scaling = m_dataset->getProperty( "scaling" ).toFloat();

    float x = m_x * dx + dx / 2.;
    float y = m_y * dy + dy / 2.;
    float z = m_z * dz + dz / 2.;


    TriangleMesh* mesh = m_spheres[lod];

    if ( zi != m_zOld || zbi != m_zbOld || m_lodOld != lod || m_scalingOld != scaling )
    {
        qDebug() << "z changed";

        QVector< QVector3D > vertices = mesh->getVertices();
        QVector< Triangle > triangles = mesh->getTriangles();

        QVector<ColumnVector>* data = m_dataset->getDataVector();


        QVector< QVector3D > normals = mesh->getVertNormals();

        QVector<TriangleMesh*>balls;

        qDebug() << "data vector size" << data->at( 0 ).Nrows();
        qDebug() << "base vector size" << m_bases[lod].Nrows();
        Matrix m = m_bases[lod] * data->at( 0 );
        qDebug() << "radius vector size" << m.Nrows() << " " << m.Ncols();

        //omp_set_num_threads( 4 );
        qDebug() << "start creating meshes";
        //#pragma omp parallel for
        for( int yy = 0; yy < ybi; ++yy )
        {
            for ( int xx = 0; xx < xbi; ++xx )
            {
                //if ( ( fabs( data->at( xx + yy * xbi + zi * xbi * ybi )(1) ) > 0.0001 ) && xx % 2 == 0 && yy % 2 == 0 )
                if ( ( fabs( data->at( xx + yy * xbi + zi * xbi * ybi )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = data->at( xx + yy * xbi + zi * xbi * ybi );
                    ColumnVector r = m_bases[lod] * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }
                    max = max - min;
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        r(i+1) = ( r(i+1) - min ) / max;
                    }

                    TriangleMesh* newBall = new TriangleMesh( vertices.size(), triangles.size() );

                    for ( int i = 0; i < vertices.size(); ++i )
                    {
                        newBall->addVertex( i, QVector3D( r(i+1)*vertices[i].x() + xx * dx + dx / 2,
                                               r(i+1)*vertices[i].y() + yy * dy + dy / 2,
                                               r(i+1)*vertices[i].z() + z ) );


                    }
                    int numVerts = vertices.size();
                    for ( int i = 0; i < triangles.size(); ++i )
                    {
                        newBall->addTriangle( i, Triangle({triangles[i].v0,triangles[i].v1,triangles[i].v2} ) );
                    }
                    #pragma omp critical
                    {
                        balls.push_back( newBall );
                    }
                }
            }
        }
        qDebug() << "end creating meshes";

        std::vector<float>verts;
        verts.reserve( balls.size() * mesh->getVertSize() * 6 );
        std::vector<int>indexes( balls.size() * mesh->getTriSize() * 3 );
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


        qDebug() << "bind buffer 0";
        m_tris1 = triangles.size() * balls.size() * 3;
        qDebug() << m_tris1 << " " << verts.size() << " " << indexes.size();

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );

        qDebug() << "bind buffer 1";
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );
    }


    if ( yi != m_yOld || ybi != m_ybOld )
    {
        qDebug() << "y changed";
    }

    if ( xi != m_xOld  || xbi != m_xbOld )
    {
        qDebug() << "x changed";
    }

    m_xOld = xi;
    m_yOld = yi;
    m_zOld = zi;
    m_xbOld = xbi;
    m_ybOld = ybi;
    m_zbOld = zbi;
    m_lodOld = lod;
    m_scalingOld = scaling;
}
