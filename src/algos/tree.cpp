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
    m_color1( QColor( 1.0, 0, 0 ) ),
    m_color2( QColor( 0.5, 0.5, 0.5 ) ),
    m_parent( 0 )
{
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

QColor Tree::getColor1()
{
    return m_color1;
}

void Tree::setColor1( QColor& color1 )
{
    m_color1 = color1;
    if ( m_parent )
    {
        m_parent->setColor1FromChild();
    }
}

void Tree::setColor1FromChild()
{
    float r = 0;
    float g = 0;
    float b = 0;
    for( int i = 0; i < m_children.size(); ++i )
    {
        QColor c = m_children[i]->getColor1();
        r += c.red();
        g += c.green();
        b += c.blue();
    }

    m_color1 = QColor( r / m_children.size(), g / m_children.size(), b / m_children.size() );

    if ( m_parent )
    {
        m_parent->setColor1FromChild();
    }
}

QColor Tree::getColor2()
{
    return m_color2;
}

void Tree::setColor2( QColor& color2 )
{
    m_color2 = color2;
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
