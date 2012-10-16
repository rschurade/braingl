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

#include "../../algos/fmath.h"

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
    m_tris( 0 ),
    vboIds( new GLuint[ 6 ] )
{
}

TensorRenderer::~TensorRenderer()
{
}

void TensorRenderer::init()
{
    glGenBuffers( 6, vboIds );
}

void TensorRenderer::draw( QMatrix4x4 mvp_matrix, QMatrix4x4 mv_matrixInvert )
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

        GLFunctions::getShader( "superquadric" )->bind();
        // Set modelview-projection matrix
        GLFunctions::getShader( "superquadric" )->setUniformValue( "mvp_matrix", mvp_matrix );
        GLFunctions::getShader( "superquadric" )->setUniformValue( "mv_matrixInvert", mv_matrixInvert );


        initGeometry();

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
        glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
        setShaderVars();

        GLfloat lightpos[] = {0.0, 0.0, 1., 0.};
        glLightfv(GL_LIGHT0, GL_POSITION, lightpos);


        glDrawElements( GL_TRIANGLES, m_tris, GL_UNSIGNED_INT, 0 );


        GLenum error;
        int i = 0;
        while ((error = glGetError()) != GL_NO_ERROR) {
            qDebug() << error;
            i++;
        }

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
}

void TensorRenderer::setupTextures()
{
}

void TensorRenderer::setShaderVars()
{
    GLFunctions::setShaderVars( "superquadric", model() );
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

    int bValue = m_dataset->getProperty( "bValue" ).toInt();

    //float scaling = m_dataset->getProperty( "scaling" ).toFloat();

    QString s = createSettingsString( xi, yi, zi, orient, 0, 0, 0, 0, 0, 0, false, bValue, 0);
    if ( s == m_previousSettings || orient == 0 )
    {
        return;
    }
    m_previousSettings = s;

    qDebug() << "Tensor Renderer: begin init geometry";

    std::vector<float>verts;
    std::vector<int>indexes;


    QVector<Matrix>* data = m_dataset->getData();
    RowVector v1( 3 );
    ColumnVector v2( 3 );

    ColumnVector newVert( 3 );

    float x = (float)xi * dx + dx / 2.;
    float y = (float)yi * dy + dy / 2.;
    float z = (float)zi * dz + dz / 2.;

    int offset = 0;

    if ( orient == 5 )
    {
        for( int yy = 0; yy < ny; ++yy )
        {
            for ( int xx = 0; xx < nx; ++xx )
            {
                Matrix tensor = data->at( xx + yy * nx + zi * nx * ny ) * 1000;

                float locX = xx * dx + dx / 2;
                float locY = yy * dy + dy / 2;

                addGlyph( &verts, &indexes, offset, locX, locY, z, tensor );
                offset += 24;
            }
        }
    }
    else if ( orient == 2 )
    {

    }
    else if ( orient == 3 )
    {

    }
    else
    {
        return;
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW );

    m_tris = indexes.size() * 3;
    qDebug() << "Tensor Renderer: end init geometry";
}

void TensorRenderer::addGlyph( std::vector<float>* verts, std::vector<int>* indexes, int offset, float xPos, float yPos, float zPos, Matrix tensor )
{
    float d0 = tensor( 1, 1 );
    float d1 = tensor( 2, 2 );
    float d2 = tensor( 3, 3 );
    float o0 = tensor( 1, 2 );
    float o1 = tensor( 1, 3 );
    float o2 = tensor( 2, 3 );

    // Front Face
    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    // Back Face
    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    // Top Face
    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    // Bottom Face
    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    // Left Face
    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    // Right Face
    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );

    verts->push_back( xPos );
    verts->push_back( yPos );
    verts->push_back( zPos );
    verts->push_back( 1.0 );
    verts->push_back( -1.0 );
    verts->push_back( 1.0 );
    verts->push_back( d0 );
    verts->push_back( d1 );
    verts->push_back( d2 );
    verts->push_back( o0 );
    verts->push_back( o1 );
    verts->push_back( o2 );


    for ( int i = 0; i < 6; ++i )
    {
        indexes->push_back( offset );
        indexes->push_back( offset + 1 );
        indexes->push_back( offset + 2 );
        indexes->push_back( offset );
        indexes->push_back( offset + 2 );
        indexes->push_back( offset + 3 );
        offset += 4;
    }
}

void TensorRenderer::setView( int view )
{
    if ( m_dataset )
    {
        m_dataset->setProperty( "renderSlice", view );
    }
}
