/*
 * octree.h
 *
 *  Created on: Apr 11, 2013
 *      Author: schurade
 */

#ifndef OCTREE_H_
#define OCTREE_H_

#include <QVector>
#include <QVector3D>

class OcTree
{
public:
    OcTree( QVector3D center, float size );
    virtual ~OcTree();

    int insert( int id, QVector3D pos );

    int pick( QVector3D pos, int parentId );

private:
    int insertLeaf( int id, QVector3D pos );

    QVector3D m_center;
    float m_size;
    QVector<OcTree*>m_leafs;

    int m_id;
    QVector3D m_pos;

};

#endif /* OCTREE_H_ */
