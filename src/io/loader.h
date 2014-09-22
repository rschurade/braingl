/*
 * Loader.h
 *
 * Created on: May 3, 2012
 * @author Ralph Schurade
 */

#ifndef LOADER_H_
#define LOADER_H_

#include "../data/datasets/dataset.h"
#include "../data/datasets/datasetmesh.h"

#include <QDir>
#include <QVector>

class Loader
{
public:
    Loader(Dataset* selected = NULL);
    virtual ~Loader();

    bool load();

    Dataset* getDataset( int id = 0 );
    int getNumDatasets();

    void setFilename( QDir fn );
    bool succes();
private:
    Fn::DatasetType determineType();

    bool loadNifti();

    bool loadVTK();
    bool loadASC( QVector3D offset = QVector3D(0,0,0) );
    bool loadSet();
    bool loadGlyphset();
    bool loadCons();
    bool loadMEG();
    bool loadTree();
    bool loadRGB();
    bool load1D();
    bool loadMRtrix();
    bool loadPNG();
    bool loadJSON();

    QDir m_fileName;

    Fn::DatasetType m_datasetType;
    std::vector<Dataset*> m_dataset;
    Dataset* m_selectedDataset;

    bool m_success;
};

#endif /* LOADER_H_ */
