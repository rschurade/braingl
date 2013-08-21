/*
 * octree.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: schurade
 */

#include "octree.h"

#include "../../gui/gl/glfunctions.h"

#include <QDebug>

OcTree::OcTree( QVector3D center, float size ) :
    m_center( center ),
    m_size( size ),
    m_id( -1 )
{
    if ( size < 0.005 ) exit( 0 );
}

OcTree::~OcTree()
{
}

int OcTree::insert( int id, QVector3D pos )
{
    if ( m_id == - 1 )
    {
        m_id = id;
        m_pos = pos;
    }
    else
    {
        if ( m_leafs.size() == 0 )
        {
            if ( m_size > 0.01 )
            {
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() + m_size/2, m_center.y() + m_size/2, m_center.z() + m_size/2 ), m_size/2 ) );
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() + m_size/2, m_center.y() + m_size/2, m_center.z() - m_size/2 ), m_size/2 ) );
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() + m_size/2, m_center.y() - m_size/2, m_center.z() + m_size/2 ), m_size/2 ) );
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() + m_size/2, m_center.y() - m_size/2, m_center.z() - m_size/2 ), m_size/2 ) );
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() - m_size/2, m_center.y() + m_size/2, m_center.z() + m_size/2 ), m_size/2 ) );
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() - m_size/2, m_center.y() + m_size/2, m_center.z() - m_size/2 ), m_size/2 ) );
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() - m_size/2, m_center.y() - m_size/2, m_center.z() + m_size/2 ), m_size/2 ) );
                m_leafs.push_back( new OcTree( QVector3D( m_center.x() - m_size/2, m_center.y() - m_size/2, m_center.z() - m_size/2 ), m_size/2 ) );

                insertLeaf( m_id, m_pos );
                return insertLeaf( id, pos );
            }
            else
            {
                return m_id;
            }
        }
        else
        {
            return insertLeaf( id, pos );
        }
    }
    return -1;
}

int OcTree::insertLeaf( int id, QVector3D pos )
{
    if ( pos.x() >= m_center.x() )
    {
        if( pos.y() >= m_center.y() )
        {
            if( pos.z() >= m_center.z() )
            {
                return m_leafs[0]->insert( id, pos );
            }
            else
            {
                return m_leafs[1]->insert( id, pos );
            }
        }
        else
        {
            if( pos.z() >= m_center.z() )
            {
                return m_leafs[2]->insert( id, pos );
            }
            else
            {
                return m_leafs[3]->insert( id, pos );
            }
        }
    }
    else
    {
        if( pos.y() >= m_center.y() )
        {
            if( pos.z() >= m_center.z() )
            {
                return m_leafs[4]->insert( id, pos );
            }
            else
            {
                return m_leafs[5]->insert( id, pos );
            }
        }
        else
        {
            if( pos.z() >= m_center.z() )
            {
                return m_leafs[6]->insert( id, pos );
            }
            else
            {
                return m_leafs[7]->insert( id, pos );
            }
        }
    }
}

int OcTree::pick( QVector3D pos, int parentId )
{
    if ( m_leafs.size() == 0 )
    {
        if ( m_id == -1 )
        {
            return parentId;
        }
        else
        {
            return m_id;
        }
    }
    else
    {
        if ( pos.x() >= m_center.x() )
        {
            if( pos.y() >= m_center.y() )
            {
                if( pos.z() >= m_center.z() )
                {
                    return m_leafs[0]->pick( pos, m_id );
                }
                else
                {
                    return m_leafs[1]->pick( pos, m_id );
                }
            }
            else
            {
                if( pos.z() >= m_center.z() )
                {
                    return m_leafs[2]->pick( pos, m_id );
                }
                else
                {
                    return m_leafs[3]->pick( pos, m_id );
                }
            }
        }
        else
        {
            if( pos.y() >= m_center.y() )
            {
                if( pos.z() >= m_center.z() )
                {
                    return m_leafs[4]->pick( pos, m_id );
                }
                else
                {
                    return m_leafs[5]->pick( pos, m_id );
                }
            }
            else
            {
                if( pos.z() >= m_center.z() )
                {
                    return m_leafs[6]->pick( pos, m_id );
                }
                else
                {
                    return m_leafs[7]->pick( pos, m_id );
                }
            }
        }
    }
}
