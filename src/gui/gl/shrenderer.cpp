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
    m_xb( 0. ),
    m_yb( 0. ),
    m_zb( 0. ),
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
        QModelIndex index = model()->index( i, FNDSE_ACTIVE );
        if ( model()->data( index, Qt::EditRole ).toBool() )
        {
            index = model()->index( i, FNDSP_CREATED_BY );
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

void SHRenderer::setupTextures()
{
}

void SHRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "qball", model() );
}

void SHRenderer::initGeometry()
{
    int xi = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toInt();
    int xbi = model()->data( model()->index( 0, FNGLOBAL_MAX_SAGITTAL ), Qt::UserRole ).toInt();
    int ybi = model()->data( model()->index( 0, FNGLOBAL_MAX_CORONAL ), Qt::UserRole ).toInt();
    int zbi = model()->data( model()->index( 0, FNGLOBAL_MAX_AXIAL ), Qt::UserRole ).toInt();

    float dx = model()->data( model()->index( 0, FNGLOBAL_SLICE_DX ), Qt::UserRole ).toFloat();
    float dy = model()->data( model()->index( 0, FNGLOBAL_SLICE_DY ), Qt::UserRole ).toFloat();
    float dz = model()->data( model()->index( 0, FNGLOBAL_SLICE_DZ ), Qt::UserRole ).toFloat();

    int lowerX = m_dataset->getProperty( "renderLowerX" ).toInt();
    int lowerY = m_dataset->getProperty( "renderLowerY" ).toInt();
    int lowerZ = m_dataset->getProperty( "renderLowerZ" ).toInt();
    int upperX = m_dataset->getProperty( "renderUpperX" ).toInt();
    int upperY = m_dataset->getProperty( "renderUpperY" ).toInt();
    int upperZ = m_dataset->getProperty( "renderUpperZ" ).toInt();

    int order = m_dataset->getProperty( "order" ).toInt();
    int lod = m_dataset->getProperty( "lod" ).toInt();

    float scaling = m_dataset->getProperty( "scaling" ).toFloat();
    int orient = m_dataset->getProperty( "renderSlice" ).toInt();

    bool minmaxScaling = m_dataset->getProperty( "minmaxScaling" ).toBool();

    QString s = createSettingsString( xi, yi, zi, lod, orient, lowerX, upperX, lowerY, upperY, lowerZ, upperZ, minmaxScaling);
    if ( s == m_previousSettings )
    {
        return;
    }
    m_previousSettings = s;

    float x = (float)xi * dx + dx / 2.;
    float y = (float)yi * dy + dy / 2.;
    float z = (float)zi * dz + dz / 2.;


    TriangleMesh* mesh = m_spheres[lod];
    QVector< QVector3D > normals = mesh->getVertNormals();

    const Matrix* v1 = tess::vertices( lod );
    Matrix base = ( QBall::sh_base( (*v1), order ) );

    std::vector<float>verts;
    std::vector<int>indexes;

    int numVerts = mesh->getVertSize();
    int currentBall = 0;

    if ( orient == 1 )
    {
        qDebug() << "QBall: start init geometry";
        QVector< QVector3D > vertices = mesh->getVertices();
        QVector< Triangle > triangles = mesh->getTriangles();

        QVector<ColumnVector>* data = m_dataset->getData();

        int glyphs = ( upperX - lowerX ) * ( upperY - lowerY );
        verts.reserve( mesh->getVertSize() * glyphs * 10 );
        indexes.reserve( triangles.size() * glyphs * 3 );

        Matrix m = base * data->at( 0 );

        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                if ( ( fabs( data->at( xx + yy * xbi + zi * xbi * ybi )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = data->at( xx + yy * xbi + zi * xbi * ybi );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }

                    if ( minmaxScaling )
                    {
                        max = max - min;
                        for ( int i = 0; i < r.Nrows(); ++i )
                        {
                            r(i+1) = ( r(i+1) - min ) / max * 0.8;
                        }
                    }
                    else
                    {
                        for ( int i = 0; i < r.Nrows(); ++i )
                        {
                            //r(i+1) *= -1.0;
                            //if ( r( i + 1) < 0.0 ) r( i+1 ) = 0.0;
                            r(i+1) = r(i+1) / max * 0.8;
                        }
                    }

                    float locX = xx * dx + dx / 2;
                    float locY =  + yy * dy + dy / 2;

                    for ( int i = 0; i < vertices.size(); ++i )
                    {
                        verts.push_back( vertices[i].x() );
                        verts.push_back( vertices[i].y() );
                        verts.push_back( vertices[i].z() );
                        verts.push_back( normals[i].x() );
                        verts.push_back( normals[i].y() );
                        verts.push_back( normals[i].z() );
                        verts.push_back( locX );
                        verts.push_back( locY );
                        verts.push_back( z );
                        verts.push_back( r(i + 1) );
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
    }
    else if ( orient == 2 )
    {
        qDebug() << "QBall: start init geometry";
        QVector< QVector3D > vertices = mesh->getVertices();
        QVector< Triangle > triangles = mesh->getTriangles();

        QVector<ColumnVector>* data = m_dataset->getData();

        Matrix m = base * data->at( 0 );

        for( int zz = lowerZ; zz < upperZ; ++zz )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                if ( ( fabs( data->at( xx + yi * xbi + zz * xbi * ybi )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = data->at( xx + yi * xbi + zz * xbi * ybi );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }
                    if ( minmaxScaling )
                    {
                        max = max - min;
                        for ( int i = 0; i < r.Nrows(); ++i )
                        {
                            r(i+1) = ( r(i+1) - min ) / max;
                        }
                    }
                    else
                    {
                        for ( int i = 0; i < r.Nrows(); ++i )
                        {
                            r(i+1) = r(i+1) / max;
                        }
                    }

                    float locX = xx * dx + dx / 2;
                    float locZ = zz * dz + dz / 2;

                    for ( int i = 0; i < vertices.size(); ++i )
                    {
                        verts.push_back( vertices[i].x() );
                        verts.push_back( vertices[i].y() );
                        verts.push_back( vertices[i].z() );
                        verts.push_back( normals[i].x() );
                        verts.push_back( normals[i].y() );
                        verts.push_back( normals[i].z() );
                        verts.push_back( locX );
                        verts.push_back( y );
                        verts.push_back( locZ );
                        verts.push_back( r(i + 1) );
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
    }
    else if ( orient == 3 )
    {
        qDebug() << "QBall: start init geometry";
        QVector< QVector3D > vertices = mesh->getVertices();
        QVector< Triangle > triangles = mesh->getTriangles();

        QVector<ColumnVector>* data = m_dataset->getData();

        Matrix m = base * data->at( 0 );

        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int zz = lowerZ; zz < upperZ; ++zz )
            {
                if ( ( fabs( data->at( xi + yy * xbi + zz * xbi * ybi )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = data->at( xi + yy * xbi + zz * xbi * ybi );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }
                    if ( minmaxScaling )
                    {
                        max = max - min;
                        for ( int i = 0; i < r.Nrows(); ++i )
                        {
                            r(i+1) = ( r(i+1) - min ) / max;
                        }
                    }
                    else
                    {
                        for ( int i = 0; i < r.Nrows(); ++i )
                        {
                            r(i+1) = r(i+1) / max;
                        }
                    }

                    float locY = yy * dy + dy / 2;
                    float locZ = zz * dz + dz / 2;

                    for ( int i = 0; i < vertices.size(); ++i )
                    {
                        verts.push_back( vertices[i].x() );
                        verts.push_back( vertices[i].y() );
                        verts.push_back( vertices[i].z() );
                        verts.push_back( normals[i].x() );
                        verts.push_back( normals[i].y() );
                        verts.push_back( normals[i].z() );
                        verts.push_back( x );
                        verts.push_back( locY );
                        verts.push_back( locZ );
                        verts.push_back( r(i + 1) );
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

    qDebug() << "QBall: end init geometry";
}
