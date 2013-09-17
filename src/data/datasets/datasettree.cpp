/*
 * datasettree.cpp
 *
 *  Created on: 17.09.2013
 *      Author: Ralph
 */

#include "datasettree.h"

#include "../../gui/gl/colormapfunctions.h"
#include "../../algos/colormapbase.h"


#include "../../algos/tree.h"

#include <QDebug>

DatasetTree::DatasetTree( QDir fn ) :
    Dataset( fn , Fn::DatasetType::TREE ),
    m_tree( 0 ),
    m_numLeaves( 0 ),
    m_numNodes( 0 )
{
}

DatasetTree::~DatasetTree()
{
}

void DatasetTree::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{

}

void DatasetTree::importTree( QString dims, QVector<QString>coords, QVector<QString>clusters )
{
    QVector<Tree*>nodes;
    QVector<Tree*>leaves( coords.size() );

    m_numLeaves = coords.size();
    m_numNodes = clusters.size();

    qDebug() << "num leaves:" << m_numLeaves << "num nodes:" << m_numNodes;

    int currentId = coords.size();

    ColormapBase cmap = ColormapFunctions::getColormap( 1 );

    for( int i = 0; i < clusters.size(); ++i )
    {
        QStringList cl = clusters[i].split( " " );
        float value = cl[0].toFloat();

        m_tree = new Tree( currentId++, value );

        nodes.push_back( m_tree );

        for ( int k = 1; k < cl.size(); k += 2 )
        {
            bool isNode = cl[k].toInt();
            int id = cl[k+1].toInt();
            if( isNode )
            {
                if( id < nodes.size() )
                {
                    nodes[id]->setParent( m_tree );
                    m_tree->addChild( nodes[id] );
                }
                else
                {
                    qDebug() << "node id error";
                }
            }
            else
            {
                Tree* leaf = new Tree( id, 0.0 );
                leaf->setParent( m_tree );
                m_tree->addChild( leaf );

                leaves[id] = leaf;
            }
        }
    }

    for( int i = 0; i < leaves.size(); ++i )
    {

        float v = ( (float)i/(float)m_numLeaves );
        QColor c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, v ) ) );
        leaves[i]->setColor1( c );
    }

    nodes.clear();
    qDebug() << "test num leaves:" << m_tree->getNumLeaves();
}
