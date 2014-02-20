/*
 * tree.cpp
 *
 *  Created on: 17.09.2013
 *      Author: Ralph
 */

#include "tree.h"

#include <QDebug>

Tree::Tree(  int id, float value ) :
    m_id( id ),
    m_value( value ),
    m_texturePosition( QVector3D( 0, 0, 0 ) ),
    m_parent( 0 )
{
    QColor color1( 1.0, 0, 0 );
    QColor color2( 0.5, 0.5, 0.5 );

    m_colors.push_back( color1 );
    m_colors.push_back( color2 );
    m_colors.push_back( color2 );
    m_colors.push_back( color2 );
}

Tree::~Tree()
{
}

Tree* Tree::getParent()
{
    return m_parent;
}

void Tree::setParent( Tree* parent )
{
    m_parent = parent;
}

void Tree::addChild( Tree* child )
{
    m_children.push_back( child );
}

QList<Tree*> Tree::getChildren()
{
    return m_children;
}

QColor Tree::getColor( int id )
{
    return m_colors[id];
}

void Tree::setColor( int id, QColor& color, bool propagateUp, bool propagateDown )
{
    m_colors[id] = color;
    if ( propagateUp && m_parent )
    {
        m_parent->setColor( id, color, true, false );
    }
    if ( propagateDown )
    {
        for ( int i = 0; i < m_children.size(); ++i )
        {
            m_children[i]->setColor( id, color, false, true );
        }
    }
}

void Tree::setColor( int id, int colorId, QColor& color )
{
    if ( id == m_id )
    {
        m_colors[colorId] = color;
        for ( int i = 0; i < m_children.size(); ++i )
        {
            m_children[i]->setColor( m_children[i]->getId(), colorId, color );
        }
    }
    else
    {
        for ( int i = 0; i < m_children.size(); ++i )
        {
            m_children[i]->setColor( id, colorId, color );
        }
    }
}

int Tree::getId()
{
    return m_id;
}

float Tree::getValue()
{
    return m_value;
}

void Tree::setValue( float value )
{
    m_value = value;
}

int Tree::getNumLeaves()
{
    int numLeaves = 0;

    if ( m_children.size() > 0 )
    {
        for ( int i = 0; i < m_children.size(); ++i )
        {
            numLeaves += m_children[i]->getNumLeaves();
        }
    }
    else
    {
        numLeaves = 1;
    }
    return numLeaves;
}

QVector3D Tree::getTexturePosition()
{
    return m_texturePosition;
}

void Tree::setTexturePosition( QVector3D value )
{
    m_texturePosition = value;
}
