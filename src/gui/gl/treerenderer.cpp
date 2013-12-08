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

TreeRenderer::TreeRenderer( QString name, Tree* tree ) :
    ObjectRenderer(),
    m_name( name ),
    m_tree( tree ),
    vboIds( new GLuint[ 2 ] ),
    m_width( 0 ),
    m_height( 0 ),
    m_dirty( true ),
    m_colorIndex( 0 ),
    m_selected( -1 )
{
}

TreeRenderer::~TreeRenderer()
{
}

void TreeRenderer::init()
{
    glGenBuffers( 2, vboIds );
}

void TreeRenderer::initGL()
{
#if 0
    GLFunctions::loadShaders();                 // XXX
    GLFunctions::getShader( "line" )->bind();   // XXX
    GLFunctions::validateShader( "line" );      // XXX
#endif
}

void TreeRenderer::draw( QMatrix4x4 mvpMatrix )
{
    if( m_dirty )
    {
        initGeometry();
    }

    if ( !m_tree )
    {
        return;
    }

    GLFunctions::getShader( "line" )->bind();
    GLFunctions::getShader( "line" )->setUniformValue( "mvp_matrix", mvpMatrix );

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
        Tree* child = children[i];

        int size = child->getNumLeaves();
        //qDebug() << size << child->getValue();

        if ( size > 1 )
        {
            m_verts.push_back( left + ( right - left ) / 2 );
            m_verts.push_back( tree->getValue() );
            m_verts.push_back( 0.0 );

            m_verts.push_back( left + offset + size / 2 );
            m_verts.push_back( tree->getValue() );
            m_verts.push_back( 0.0 );

            m_verts.push_back( left + offset + size / 2 );
            m_verts.push_back( tree->getValue() );
            m_verts.push_back( 0.0 );

            m_verts.push_back( left + offset + size / 2 );
            m_verts.push_back( child->getValue() );
            m_verts.push_back( 0.0 );

            QColor color = tree->getColor( m_colorIndex );

            if ( tree->getId() == m_selected )
            {
                QColor color1 = tree->getColor( 1 );
                m_colors.push_back( color1.redF() );
                m_colors.push_back( color1.greenF() );
                m_colors.push_back( color1.blueF() );
                m_colors.push_back( 1.0 );
                m_colors.push_back( color1.redF() );
                m_colors.push_back( color1.greenF() );
                m_colors.push_back( color1.blueF() );
                m_colors.push_back( 1.0 );
            }
            else
            {
                m_colors.push_back( color.redF() );
                m_colors.push_back( color.greenF() );
                m_colors.push_back( color.blueF() );
                m_colors.push_back( 1.0 );
                m_colors.push_back( color.redF() );
                m_colors.push_back( color.greenF() );
                m_colors.push_back( color.blueF() );
                m_colors.push_back( 1.0 );
            }

            m_colors.push_back( color.redF() );
            m_colors.push_back( color.greenF() );
            m_colors.push_back( color.blueF() );
            m_colors.push_back( 1.0 );
            m_colors.push_back( color.redF() );
            m_colors.push_back( color.greenF() );
            m_colors.push_back( color.blueF() );
            m_colors.push_back( 1.0 );
        }
        else
        {
            m_verts.push_back( left + ( right - left ) / 2 );
            m_verts.push_back( tree->getValue() );
            m_verts.push_back( 0.0 );

            m_verts.push_back( left + offset + size / 2 );
            m_verts.push_back( child->getValue() );
            m_verts.push_back( 0.0 );

            QColor color = tree->getColor( m_colorIndex );

            m_colors.push_back( color.redF() );
            m_colors.push_back( color.greenF() );
            m_colors.push_back( color.blueF() );
            m_colors.push_back( 1.0 );
            m_colors.push_back( color.redF() );
            m_colors.push_back( color.greenF() );
            m_colors.push_back( color.blueF() );
            m_colors.push_back( 1.0 );
        }

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
