/*
 * singleshrenderer.cpp
 *
 *  Created on: 12.07.2012
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
#include "arcball.h"

#include "singleshrenderer.h"

SingleSHRenderer::SingleSHRenderer( QString name ) :
    ObjectRenderer(),
    vboIds( new GLuint[ 2 ] ),
    m_name( name ),
    m_width( 1 ),
    m_height( 1 ),
    m_ratio( 1.0 ),
    m_x( 0. ),
    m_y( 0. ),
    m_z( 0. ),
    m_xb( 0 ),
    m_yb( 0 ),
    m_zb( 0 )
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

    m_arcBall = new ArcBall( 50, 50 );
}

SingleSHRenderer::~SingleSHRenderer()
{
}

void SingleSHRenderer::init()
{
}

void SingleSHRenderer::initGL()
{
    qDebug() << "gl init " << m_name << " widget";
    GLenum errorCode = glewInit();
    if ( GLEW_OK != errorCode )
    {
        qDebug() << "Problem: glewInit failed, something is seriously wrong.";
        qDebug() << glewGetErrorString( errorCode );
        exit( false );
    }
    else
    {
        //qDebug() << "OpenGL initialized.";
    }

    glGenBuffers( 2, vboIds );

    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );
    static GLfloat lightPosition[ 4 ] = { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    GLFunctions::loadShaders();
}

void SingleSHRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;

    m_arcBall->set_win_size( width, height );

    calcMVPMatrix();
}

void SingleSHRenderer::leftMouseDown( int x, int y )
{
    m_arcBall->click( x, y );
    calcMVPMatrix();
}

void SingleSHRenderer::leftMouseDrag( int x, int y )
{
    m_arcBall->drag( x, y );
    calcMVPMatrix();
}

void SingleSHRenderer::calcMVPMatrix()
{
    glViewport( 0, 0, m_width, m_height );
    m_ratio = static_cast< float >( m_width ) / static_cast< float >( m_height );

    m_arcBall->setRotCenter( 1., 1., 1. );

    // Reset projection
    QMatrix4x4 pMatrix;
    pMatrix.setToIdentity();


    if ( m_ratio >= 1.0 )
    {
        pMatrix.ortho( -m_ratio, m_ratio, -1., 1., -3000, 3000 );
    }
    else
    {
        pMatrix.ortho( -1., 1., -1. / m_ratio, 1. / m_ratio, -3000, 3000 );

    }

    m_mvpMatrix = pMatrix * m_arcBall->getMVMat();
}


void SingleSHRenderer::initGeometry()
{
    m_x = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toFloat();
    int xi = model()->data( model()->index( 0, 100 ), Qt::UserRole ).toInt();
    int yi = model()->data( model()->index( 0, 101 ), Qt::UserRole ).toInt();
    int zi = model()->data( model()->index( 0, 102 ), Qt::UserRole ).toInt();
    int xbi = model()->data( model()->index( 0, 103 ), Qt::UserRole ).toInt();
    int ybi = model()->data( model()->index( 0, 104 ), Qt::UserRole ).toInt();
    //int zbi = model()->data( model()->index( 0, 105 ), Qt::UserRole ).toInt();

    int lod = 4; //m_dataset->getProperty( "lod" ).toInt();
    //float scaling = m_dataset->getProperty( "scaling" ).toFloat();

    TriangleMesh* mesh = m_spheres[lod];

    int order = m_dataset->getProperty( "order" ).toInt();

    const Matrix* v1 = tess::vertices( lod );
    Matrix base = ( QBall::sh_base( (*v1), order ) );

    QString s = createSettingsString( xi, yi, zi, 0, 0, 0, 0, 0, 0, 0, false, 0);
    if ( s == m_previousSettings )
    {
        return;
    }
    m_previousSettings = s;

    QVector< QVector3D > vertices = mesh->getVertices();
    QVector< Triangle > triangles = mesh->getTriangles();

    QVector<ColumnVector>* data = m_dataset->getData();


    QVector< QVector3D > normals = mesh->getVertNormals();

    Matrix m = base * data->at( 0 );

    std::vector<float>verts;
    verts.reserve( mesh->getVertSize() * 10 );
    std::vector<int>indexes;
    indexes.reserve( mesh->getTriSize() * 3 );

    if ( ( fabs( data->at( xi + yi * xbi + zi * xbi * ybi )(1) ) > 0.0001 ) )
    {
        ColumnVector dv = data->at( xi + yi * xbi + zi * xbi * ybi );
        ColumnVector r = base * dv;

        float max = 0;
        float min = std::numeric_limits<float>::max();
        for ( int i = 0; i < r.Nrows(); ++i )
        {
            max = qMax( max, (float)r(i+1) );
            min = qMin( min, (float)r(i+1) );
        }

        bool minmaxScaling = m_dataset->getProperty( "minmaxScaling" ).toBool();
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

        for ( int i = 0; i < vertices.size(); ++i )
        {
            verts.push_back( vertices[i].x() );
            verts.push_back( vertices[i].y() );
            verts.push_back( vertices[i].z() );
            verts.push_back( normals[i].x() );
            verts.push_back( normals[i].y() );
            verts.push_back( normals[i].z() );
            verts.push_back( 1.0 );
            verts.push_back( 1.0 );
            verts.push_back( 1.0 );
            verts.push_back( r(i + 1) );
        }
        for ( int i = 0; i < triangles.size(); ++i )
        {
            indexes.push_back( triangles[i].v0 );
            indexes.push_back( triangles[i].v1 );
            indexes.push_back( triangles[i].v2 );
        }
    }

    m_tris1 = triangles.size() * 3;
    //qDebug() << m_tris1 << " " << verts.size() << " " << indexes.size();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );

}

void SingleSHRenderer::setupTextures()
{
}

void SingleSHRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "qball", model() );
}

void SingleSHRenderer::draw()
{
    QList<int>rl;

    int countDatasets = model()->rowCount();
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
        m_dataset = VPtr<DatasetDWI>::asPtr( model()->data( model()->index( rl[0], 2 ), Qt::EditRole ) );

        GLFunctions::getShader( "qball" )->bind();
        // Set modelview-projection matrix
        GLFunctions::getShader( "qball" )->setUniformValue( "mvp_matrix", m_mvpMatrix );

        initGeometry();

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        setShaderVars();
        glDrawElements( GL_TRIANGLES, m_tris1, GL_UNSIGNED_INT, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
}
