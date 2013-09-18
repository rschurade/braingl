/*
 * tree.h
 *
 *  Created on: 17.09.2013
 *      Author: Ralph
 */

#ifndef TREE_H_
#define TREE_H_

#include <QColor>
#include <QList>
#include <QVector>
#include <QVector3D>

class Tree
{
public:
    Tree( int id, float value );
    virtual ~Tree();

    QList<Tree*> getChildren();
    void addChild( Tree* child );

    QColor getColor( int id );
    void setColor( int id, QColor& color, bool propagateUp, bool propagateDown );

    int getId();

    Tree* getParent();
    void setParent( Tree* parent );

    float getValue();
    void setValue( float value );

    QVector3D getTexturePosition();
    void setTexturePosition( QVector3D value );

    int getNumLeaves();

private:
    int m_id;
    float m_value;   // some value, may be interpreted as height of the node, thus leafs have 0.0 and the root <= 1.0
    QVector<QColor> m_colors; // color that is assigned at the beginning based on leaf id
    QVector3D m_texturePosition;

    Tree* m_parent;
    QList<Tree*>m_children;
};

#endif /* TREE_H_ */
