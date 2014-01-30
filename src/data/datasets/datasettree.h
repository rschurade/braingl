/*
 * datasettree.h
 *
 *  Created on: 17.09.2013
 *      Author: Ralph
 */

#ifndef DATASETTREE_H_
#define DATASETTREE_H_

#include "dataset.h"

#include <QString>
#include <QVector>

class Tree;
class TreeRenderer;

class DatasetTree : public Dataset
{
    Q_OBJECT

public:
    DatasetTree( QDir fn = QDir( "hierarchical tree" ) );
    virtual ~DatasetTree();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );
    void drawTree( QMatrix4x4 mvpMatrix, int width, int height );

    void importTree( QString dims, std::vector<QString>coords, std::vector<QString>clusters );

    Tree* getTree() { return m_tree; };

    bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );
    void setZoom( int zoom ) { m_zoom = zoom; };

private:
    void createTexture();

    void createTextureRec( Tree* tree );
    int pickClusterRec( Tree* tree, int left, int right, float x, float y );

    Tree* m_tree;
    TreeRenderer* m_renderer;

    int m_numLeaves;
    int m_numNodes;

    std::vector<float>m_textureData;

    int m_width;
    int m_height;
    int m_picked;
    int m_zoom;

private slots:
    void selectCluster( QVariant id );
    void colorSelectionChanged( QVariant value );
    void setClusterColor();
    void partitionModeChanged( QVariant value );
    void applyPartitionMode();

};

#endif /* DATASETTREE_H_ */
