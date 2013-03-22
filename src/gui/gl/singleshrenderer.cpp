/*
 * singleshrenderer.cpp
 *
 * Created on: 12.07.2012
 * @author Ralph Schurade
 */
#include "singleshrenderer.h"

#include "glfunctions.h"
#include "arcball.h"

#include "../../data/datasets/datasetdwi.h"
#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/vptr.h"
#include "../../algos/fmath.h"
#include "../../algos/qball.h"

#include "../../data/mesh/tesselation.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>
#include <QVector3D>
#include <QMatrix4x4>

#include <limits>

SingleSHRenderer::SingleSHRenderer() :
    ObjectRenderer(),
    vboIds( new GLuint[ 2 ] ),
    m_width( 1 ),
    m_height( 1 ),
    m_ratio( 1.0 ),
    m_dx( 1.0 ),
    m_dy( 1.0 ),
    m_dz( 1.0 )
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
    qDebug() << "gl init single sh widget";
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
    m_dx = m_dataset->properties()->get( Fn::Property::DX ).toFloat();
    m_dy = m_dataset->properties()->get( Fn::Property::DY ).toFloat();
    m_dz = m_dataset->properties()->get( Fn::Property::DZ ).toFloat();

    m_nx = m_dataset->properties()->get( Fn::Property::NX ).toInt();
    m_ny = m_dataset->properties()->get( Fn::Property::NY ).toInt();
    m_nz = m_dataset->properties()->get( Fn::Property::NZ ).toInt();

    float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();

    int xi = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toFloat() * ( dx / m_dx );
    int yi = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toFloat() * ( dy / m_dy );
    int zi = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toFloat() * ( dz / m_dz );

    xi = qMax( 0, qMin( xi, m_nx - 1) );
    yi = qMax( 0, qMin( yi, m_ny - 1) );
    zi = qMax( 0, qMin( zi, m_nz - 1) );



    int lod = 4; //m_dataset->getProperty( "lod" ).toInt();

    const Matrix* vertices = tess::vertices( lod );
    const int* faces = tess::faces( lod );
    int numVerts = tess::n_vertices( lod );
    int numTris = tess::n_faces( lod );

    int order = m_dataset->properties()->get( Fn::Property::ORDER ).toInt();

    Matrix base = ( FMath::sh_base( (*vertices), order ) );

    QString s = createSettingsString( { xi, yi, zi } );

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
    m_tris = 0;

    if ( ( fabs( data->at( xi + yi * m_nx + zi * m_nx * m_ny )(1) ) > 0.0001 ) )
    {
        ColumnVector dv = data->at( xi + yi * m_nx + zi * m_nx * m_ny );
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
        m_tris = numTris * 3;
    }


    qDebug() << m_tris << " " << verts.size() << " " << indexes.size();

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
    QGLShaderProgram* program = GLFunctions::getShader( "qball" );

    program->bind();

    long int offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );


    offset += sizeof(float) * 3;
    int offsetLocation = program->attributeLocation( "a_offset" );
    program->enableAttributeArray( offsetLocation );
    glVertexAttribPointer( offsetLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );

    offset += sizeof(float) * 3;
    int radiusLocation = program->attributeLocation( "a_radius" );
    program->enableAttributeArray( radiusLocation );
    glVertexAttribPointer( radiusLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void *) offset );
}

void SingleSHRenderer::draw()
{
    QList<int>rl;

    int countDatasets = model()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = model()->index( i, (int)Fn::Property::ACTIVE );
        if ( model()->data( index, Qt::DisplayRole ).toBool() )
        {
            index = model()->index( i, (int)Fn::Property::TYPE );
            if ( model()->data( index, Qt::DisplayRole ).toInt() == (int)Fn::DatasetType::NIFTI_SH )
            {
                rl.push_back( i );
                //qDebug() << "found QBall to render";
            }
        }
    }

    if ( rl.size() > 0 )
    {
        m_dataset = VPtr<DatasetDWI>::asPtr( model()->data( model()->index( rl[0], (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
        initGeometry();
    }
    else
    {
        m_tris = 0;
    }

    if ( m_tris != 0 )
    {
        GLFunctions::getShader( "qball" )->bind();
        // Set modelview-projection matrix
        GLFunctions::getShader( "qball" )->setUniformValue( "mvp_matrix", m_mvpMatrix );
        GLFunctions::getShader( "qball" )->setUniformValue( "mv_matrixInvert", m_arcBall->getMVMat().inverted() );

        bool hnl = m_dataset->properties()->get( Fn::Property::HIDE_NEGATIVE_LOBES ).toBool();
        GLFunctions::getShader( "qball" )->setUniformValue( "u_hideNegativeLobes", hnl );

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );

        setShaderVars();

        glDrawElements( GL_TRIANGLES, m_tris, GL_UNSIGNED_INT, 0 );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
}
