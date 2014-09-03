/*
 * loadertree.h
 *
 *  Created on: 18.08.2014
 *      Author: Ralph
 */

#ifndef LOADERTREE_H_
#define LOADERTREE_H_

#include <QString>

class DatasetTree;

class LoaderTree
{
public:
    LoaderTree( QString fileName );
    virtual ~LoaderTree();

    DatasetTree* getDataset() { return m_dataset;};

    bool load();

private:
    QString m_fileName;
    DatasetTree* m_dataset;

    bool loadSurfaceMesh( QString fileName );
    bool loadProjection( QString fileName );

    float m_meshoffsetX;
    float m_meshoffsetY;
    float m_meshoffsetZ;

};

#endif /* LOADERTREE_H_ */
