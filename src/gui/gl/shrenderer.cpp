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

#include "../../data/datasets/datasetqball.h"
#include "../../data/enums.h"
#include "../../data/vptr.h"
#include "../../data/qball.h"

#include "../../data/mesh/tesselation.h"
#include "../../data/mesh/trianglemesh.h"

#include "../../thirdparty/newmat10/newmat.h"

#include "glfunctions.h"
#include "shrenderer.h"

SHRenderer::SHRenderer( QVector<ColumnVector>* data, int m_nx, int m_ny, int m_nz, float m_dx, float m_dy, float m_dz ) :
    ObjectRenderer(),
    m_tris1( 0 ),
    vboIds( new GLuint[ 6 ] ),
    m_data( data ),
    m_nx( m_nx ),
    m_ny( m_ny ),
    m_nz( m_nz ),
    m_dx( m_dx ),
    m_dy( m_dy ),
    m_dz( m_dz ),
    m_scaling( 1.0 ),
    m_orient( 0 ),
    m_offset( 0.0 ),
    m_lodAdjust( 0 ),
    m_minMaxScaling( false ),
    m_order( 4 )
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

        /*
        if ( lod < 3 )
        {
            QVector<QSet<int> > neighs = m_spheres[lod]->getNeighbors();

            for ( int i = 0; i < neighs.size(); ++i )
            {
                QSet<int>n = neighs[i];
                QString s( "" );
                foreach (const int &value, n)
                {
                    s += QString::number( value );
                    s += " ";
                }
                qDebug() << s;
            }
        }
        */
    }
}

SHRenderer::~SHRenderer()
{
}

void SHRenderer::init()
{
    glGenBuffers( 6, vboIds );
}

void SHRenderer::draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert )
{
    if ( m_orient == 0 )
    {
        return;
    }

    GLFunctions::getShader( "qball" )->bind();
    // Set modelview-projection matrix
    GLFunctions::getShader( "qball" )->setUniformValue( "mvp_matrix", mvp_matrix );
    GLFunctions::getShader( "qball" )->setUniformValue( "mv_matrixInvert", mv_matrixInvert );
    GLFunctions::getShader( "qball" )->setUniformValue( "u_hideNegativeLobes", m_minMaxScaling );


    initGeometry();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars();
    glDrawElements( GL_TRIANGLES, m_tris1, GL_UNSIGNED_INT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
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


    calcBounds( m_nx, m_ny, m_nz, m_dx, m_dy, m_dz, m_orient );

    int lowerX = m_visibleArea[0];
    int lowerY = m_visibleArea[2];
    int lowerZ = m_visibleArea[4];
    int upperX = m_visibleArea[1];
    int upperY = m_visibleArea[3];
    int upperZ = m_visibleArea[5];

    int _lod = m_lodAdjust - 2;

    QString s = createSettingsString( xi, yi, zi, m_orient, lowerX, upperX, lowerY, upperY, lowerZ, upperZ, false, 0, _lod);
    if ( s == m_previousSettings || m_orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    int lod = qMin( 5, qMax( 0, getMaxLod( m_orient, lowerX, upperX, lowerY, upperY, lowerZ, upperZ ) + _lod ) );
    qDebug() << "SH Renderer: using lod " << lod;

    float x = (float)xi * m_dx + m_dx / 2.;
    float y = (float)yi * m_dy + m_dy / 2.;
    float z = (float)zi * m_dz + m_dz / 2.;


    TriangleMesh* mesh = m_spheres[lod];
    QVector< QVector3D > normals = mesh->getVertNormals();

    // TODO
    const Matrix* v1 = tess::vertices( lod );
    Matrix base = ( QBall::sh_base( (*v1), m_order ) );

    std::vector<float>verts;
    std::vector<int>indexes;

    int numVerts = mesh->getVertSize();
    int currentBall = 0;

    if ( m_orient == 1 )
    {
        //qDebug() << "QBall: start init geometry";
        QVector< QVector3D > vertices = mesh->getVertices();
        QVector< Triangle > triangles = mesh->getTriangles();
        QVector< QSet<int> > neighbors = mesh->getNeighbors();

        int glyphs = ( upperX - lowerX ) * ( upperY - lowerY );
        verts.reserve( mesh->getVertSize() * glyphs * 10 );
        indexes.reserve( triangles.size() * glyphs * 3 );

        Matrix m = base * m_data->at( 0 );

        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                if ( ( fabs( m_data->at( xx + yy * m_nx + zi * m_nx * m_ny )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = m_data->at( xx + yy * m_nx + zi * m_nx * m_ny );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }


                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        r(i+1) = r(i+1) / max * 0.8;
                    }

                    float locX = xx * m_dx + m_dx / 2;
                    float locY = yy * m_dy + m_dy / 2;

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
    else if ( m_orient == 2 )
    {
        //qDebug() << "QBall: start init geometry";
        QVector< QVector3D > vertices = mesh->getVertices();
        QVector< Triangle > triangles = mesh->getTriangles();

        Matrix m = base * m_data->at( 0 );

        for( int zz = lowerZ; zz < upperZ; ++zz )
        {
            for ( int xx = lowerX; xx < upperX; ++xx )
            {
                if ( ( fabs( m_data->at( xx + yi * m_nx + zz * m_nx * m_ny )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = m_data->at( xx + yi * m_nx + zz * m_nx * m_ny );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }

                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        r(i+1) = r(i+1) / max;
                    }

                    float locX = xx * m_dx + m_dx / 2;
                    float locZ = zz * m_dz + m_dz / 2;

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
    else if ( m_orient == 3 )
    {
        //qDebug() << "QBall: start init geometry";
        QVector< QVector3D > vertices = mesh->getVertices();
        QVector< Triangle > triangles = mesh->getTriangles();

        Matrix m = base * m_data->at( 0 );

        for( int yy = lowerY; yy < upperY; ++yy )
        {
            for ( int zz = lowerZ; zz < upperZ; ++zz )
            {
                if ( ( fabs( m_data->at( xi + yy * m_nx + zz * m_nx * m_ny )(1) ) > 0.0001 ) )
                {
                    ColumnVector dv = m_data->at( xi + yy * m_nx + zz * m_nx * m_ny );
                    ColumnVector r = base * dv;

                    float max = 0;
                    float min = std::numeric_limits<float>::max();
                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        max = qMax( max, (float)r(i+1) );
                        min = qMin( min, (float)r(i+1) );
                    }

                    for ( int i = 0; i < r.Nrows(); ++i )
                    {
                        r(i+1) = r(i+1) / max;
                    }

                    float locY = yy * m_dy + m_dy / 2;
                    float locZ = zz * m_dz + m_dz / 2;

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

    //qDebug() << "QBall: end init geometry";
}

void SHRenderer::setRenderParams( float scaling, int orient, float offset, int lodAdjust, bool minMaxScaling, int order )
{
    m_scaling = scaling;
    m_orient = orient;
    m_offset = offset;
    m_lodAdjust = lodAdjust;
    m_minMaxScaling = minMaxScaling;
    m_order = order;
}
