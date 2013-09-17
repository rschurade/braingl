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

class Tree
{
public:
    Tree( int id, float value );
    virtual ~Tree();

    QList<Tree*> getChildren();
    void addChild( Tree* child );

    QColor getColor1();
    void setColor1( QColor& color1 );

    void setColor1FromChild();

    QColor getColor2();
    void setColor2( QColor& color2 );

    int getId();

    Tree* getParent();
    void setParent( Tree* parent );

    float getValue();
    void setValue( float value );

    int getNumLeaves();

private:
    int m_id;
    float m_value;   // some value, may be interpreted as height of the node, thus leafs have 0.0 and the root <= 1.0
    QColor m_color1; // color that is assigned at the beginning based on leaf id
    QColor m_color2; // selected color, user selected color, overrides other colors

    Tree* m_parent;
    QList<Tree*>m_children;
};

#endif /* TREE_H_ */
