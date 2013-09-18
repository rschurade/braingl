/*
 * TreeRenderer.cpp
 *
 * Created on: 17.09.2013
 * @author Ralph Schurade
 */
#include "treerenderer.h"

#include "../gl/glfunctions.h"

#include "../../algos/tree.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/vptr.h"

#include "../../data/datasets/datasettree.h"

#include <QDebug>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

TreeRenderer::TreeRenderer( QString name ) :
    ObjectRenderer(),
    m_name( name ),
    vboIds( new GLuint[ 2 ] ),
    m_tree( 0 ),
    m_dirty( true )
{
}

TreeRenderer::~TreeRenderer()
{
}

void TreeRenderer::init()
{
}

void TreeRenderer::initGL()
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

    glClearColor( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_DEPTH_TEST );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );
    static GLfloat lightPosition[ 4 ] = { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
}

void TreeRenderer::draw()
{
    glViewport( 0, 0, m_width, m_height );

    if( !m_tree || m_dirty )
    {
        initGeometry();
    }

    if ( !m_tree )
    {
        return;
    }

    QColor color = Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_NAV2, 0 ) ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

    //qDebug() << "nav draw";
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    int leaves = m_tree->getNumLeaves();

   // Reset projection
   QMatrix4x4 pMatrix;
   pMatrix.setToIdentity();


    pMatrix.ortho(  0,  leaves, 0, 1., -3000, 3000 );


    GLFunctions::getShader( "line" )->bind();
    GLFunctions::getShader( "line" )->setUniformValue( "mvp_matrix", pMatrix );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    // Draw cube geometry using indices from VBO 0
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = GLFunctions::getShader( "line" )->attributeLocation( "a_position" );
    GLFunctions::getShader( "line" )->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    // Draw cube geometry using indices from VBO 1
    int colorLocation = GLFunctions::getShader( "line" )->attributeLocation( "a_color" );
    GLFunctions::getShader( "line" )->enableAttributeArray( colorLocation );
    glVertexAttribPointer( colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0 );


    // Draw cube geometry using indices from VBO 0
    glDrawArrays( GL_LINES, 0 , m_verts.size() / 2 );

}

void TreeRenderer::initGeometry()
{
    qDebug() << "init tree geometry";
    QList<int>rl;
    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = Models::d()->index( i, (int)Fn::Property::D_ACTIVE );
        if ( Models::d()->data( index, Qt::DisplayRole ).toBool() )
        {
            index = Models::d()->index( i, (int)Fn::Property::D_TYPE );
            if ( Models::d()->data( index, Qt::DisplayRole ).toInt() == (int)Fn::DatasetType::TREE )
            {
                rl.push_back( i );
            }
        }
    }

    DatasetTree* ds = 0;
    if ( rl.size() > 0 )
    {
        ds = VPtr<DatasetTree>::asPtr( Models::d()->data( Models::d()->index( rl[0], (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
    }
    else
    {
        return;
    }

    m_tree = ds->getTree();


    m_verts.clear();
    m_colors.clear();

    initGeometryRec( m_tree, 0, m_tree->getNumLeaves() );

    // Transfer vertex data to VBO 3
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, m_verts.size() * sizeof( float ), m_verts.data(), GL_STATIC_DRAW );

    // Transfer vertex data to VBO 3
   glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
   glBufferData( GL_ARRAY_BUFFER, m_colors.size() * sizeof( float ), m_colors.data(), GL_STATIC_DRAW );

   m_dirty = false;
}

void TreeRenderer::initGeometryRec( Tree* tree, int left, int right )
{
    QList<Tree*> children = tree->getChildren();

    int offset = 0;
    for ( int i = 0; i < children.size(); ++i )
    {
        m_verts.push_back( left + ( right - left ) / 2 );
        m_verts.push_back( tree->getValue() );
        m_verts.push_back( 0.0 );

        Tree* child = children[i];

        int size = child->getNumLeaves();
        //qDebug() << size << child->getValue();
        m_verts.push_back( left + offset + size / 2 );
        m_verts.push_back( tree->getValue() );
        m_verts.push_back( 0.0 );

        m_verts.push_back( left + offset + size / 2 );
        m_verts.push_back( tree->getValue() );
        m_verts.push_back( 0.0 );

        m_verts.push_back( left + offset + size / 2 );
        m_verts.push_back( child->getValue() );
        m_verts.push_back( 0.0 );

        QColor color = tree->getColor( 0 );
        m_colors.push_back( color.redF() );
        m_colors.push_back( color.greenF() );
        m_colors.push_back( color.blueF() );
        m_colors.push_back( 1.0 );
        m_colors.push_back( color.redF() );
        m_colors.push_back( color.greenF() );
        m_colors.push_back( color.blueF() );
        m_colors.push_back( 1.0 );
        m_colors.push_back( color.redF() );
        m_colors.push_back( color.greenF() );
        m_colors.push_back( color.blueF() );
        m_colors.push_back( 1.0 );
        m_colors.push_back( color.redF() );
        m_colors.push_back( color.greenF() );
        m_colors.push_back( color.blueF() );
        m_colors.push_back( 1.0 );



        initGeometryRec( child, left + offset, left + offset + size );

        offset += size;

    }
}


void TreeRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;
}

void TreeRenderer::leftMouseDown( int x, int y )
{
}

void TreeRenderer::leftMouseDrag( int x, int y )
{
    leftMouseDown( x, y );
}

void TreeRenderer::setShaderVars()
{
}

void TreeRenderer::mouseWheel( int step )
{
//    m_zoom += step;
//    m_zoom = qMax( 1, m_zoom );
}

void TreeRenderer::middleMouseDown( int x, int y )
{
//    m_moveXOld = m_moveX;
//    m_moveYOld = m_moveY;
//    m_middleDownX = x;
//    m_middleDownY = y;
}

void TreeRenderer::middleMouseDrag( int x, int y )
{
//    m_moveX = m_moveXOld - ( m_middleDownX - x );
//    m_moveY = m_moveYOld + m_middleDownY - y;
}

void TreeRenderer::update()
{
    m_dirty = true;
}
