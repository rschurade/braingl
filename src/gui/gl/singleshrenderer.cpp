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
#include "../../algos/fmath.h"
#include "../../algos/qball.h"

#include "../../data/mesh/tesselation.h"

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
    m_x = model()->data( model()->index( 0, FNGLOBAL_SAGITTAL ), Qt::UserRole ).toFloat();
    m_y = model()->data( model()->index( 0, FNGLOBAL_CORONAL ), Qt::UserRole ).toFloat();
    m_z = model()->data( model()->index( 0, FNGLOBAL_AXIAL ), Qt::UserRole ).toFloat();
    int xi = m_x;
    int yi = m_y;
    int zi = m_z;
    int xbi = model()->data( model()->index( 0, FNGLOBAL_MAX_SAGITTAL), Qt::UserRole ).toInt();
    int ybi = model()->data( model()->index( 0, FNGLOBAL_MAX_CORONAL ), Qt::UserRole ).toInt();
    //int zbi = model()->data( model()->index( 0, FNGLOBAL_MAX_AXIAL), Qt::UserRole ).toInt();

    int lod = 4; //m_dataset->getProperty( "lod" ).toInt();

    const Matrix* vertices = tess::vertices( lod );
    const int* faces = tess::faces( lod );
    int numVerts = tess::n_vertices( lod );
    int numTris = tess::n_faces( lod );

    int order = m_dataset->properties()->get( FNPROP_ORDER ).toInt();

    Matrix base = ( FMath::sh_base( (*vertices), order ) );

    QString s = createSettingsString( xi, yi, zi, 0, 0, 0, 0, 0, 0, 0, false, 0);
    if ( s == m_previousSettings )
    {
        return;
    }
    m_previousSettings = s;

    QVector<ColumnVector>* data = m_dataset->getData();

    Matrix m = base * data->at( 0 );

    std::vector<float>verts;
    verts.reserve( numVerts * 10 );
    std::vector<int>indexes;
    indexes.reserve( numTris * 3 );

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

        for ( int i = 0; i < r.Nrows(); ++i )
        {
            r(i+1) = r(i+1) / max * 0.8;
        }

        for ( int i = 0; i < numVerts; ++i )
        {
            verts.push_back( (*vertices)( i+1, 1 ) );
            verts.push_back( (*vertices)( i+1, 2 ) );
            verts.push_back( (*vertices)( i+1, 3 ) );
            verts.push_back( (*vertices)( i+1, 1 ) );
            verts.push_back( (*vertices)( i+1, 2 ) );
            verts.push_back( (*vertices)( i+1, 3 ) );
            verts.push_back( 1.0 );
            verts.push_back( 1.0 );
            verts.push_back( 1.0 );
            verts.push_back( r(i + 1) );
        }
        for ( int i = 0; i < numTris; ++i )
        {
            indexes.push_back( faces[i*3] );
            indexes.push_back( faces[i*3+1] );
            indexes.push_back( faces[i*3+2] );
        }
    }

    m_tris1 = numTris * 3;
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
//    QList<int>rl;
//
//    int countDatasets = model()->rowCount();
//    for ( int i = 0; i < countDatasets; ++i )
//    {
//        QModelIndex index = model()->index( i, FNPROP_ACTIVE );
//        if ( model()->data( index, Qt::EditRole ).toBool() )
//        {
//            index = model()->index( i, FNPROP_TYPE );
//            if ( model()->data( index, Qt::DisplayRole ).toInt() == FNDT_NIFTI_SH )
//            {
//                rl.push_back( i );
//                //qDebug() << "found QBall to render";
//            }
//        }
//    }
//
//    if ( rl.size() > 0 )
//    {
//        m_dataset = VPtr<DatasetDWI>::asPtr( model()->data( model()->index( rl[0], FNPROP_DATASET_POINTER ), Qt::EditRole ) );
//
//        GLFunctions::getShader( "qball" )->bind();
//        // Set modelview-projection matrix
//        GLFunctions::getShader( "qball" )->setUniformValue( "mvp_matrix", m_mvpMatrix );
//        GLFunctions::getShader( "qball" )->setUniformValue( "mv_matrixInvert", m_mvpMatrix.inverted() );
//
//        bool minMaxScaling = m_dataset->getProperty( FNPROP_MINMAX_SCALING ).toBool();
//        GLFunctions::getShader( "qball" )->setUniformValue( "u_hideNegativeLobes", minMaxScaling );
//
//        initGeometry();
//
//        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
//        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
//        setShaderVars();
//        glDrawElements( GL_TRIANGLES, m_tris1, GL_UNSIGNED_INT, 0 );
//        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//        glBindBuffer( GL_ARRAY_BUFFER, 0 );
//    }
}
