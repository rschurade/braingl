/*
 * datasetconglyphs.h
 *
 *  Created on: Oct 30, 2013
 *      Author: boettgerj
 */

#ifndef DATASETCONGLYPHS_H_
#define DATASETCONGLYPHS_H_

#include "dataset.h"

#include "../../algos/connections.h"

#include "../../gui/gl/pointglyphrenderer.h"
#include "../../gui/gl/diffpointglyphrenderer.h"
#include "../../gui/gl/vectorglyphrenderer.h"
#include "../../gui/gl/pieglyphrenderer.h"

#include "../../gui/gl/meshrenderer.h"

#include <qstring.h>

class DatasetConGlyphs: public Dataset
{
    Q_OBJECT
public:
    DatasetConGlyphs(QDir filename);
    virtual ~DatasetConGlyphs();

    void addProperties();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

    void setProperties();

    void makeCons();
    int consNumber;

    void makeVecs();
    int vecsNumber;

    void makePies();
    int maxNodeCount;

    void makeDiffPoints();
    int diffsNumber;

    void setCons(Connections* m_cons);

private:

    Connections* m_cons;

    int m_n;  //number of vertices

    float* consArray;
    float* diffsArray;
    float* vecsArray;

    std::vector<float*>* pieArrays;
    std::vector<int>* numbers;

    PointGlyphRenderer* m_prenderer;
    VectorGlyphRenderer* m_vrenderer;
    PieGlyphRenderer* m_pierenderer;

    int prevGlyphstyle, prevColorMode;
    float prevThresh, prevMinlength;

private slots:
    void glyphStyleChanged( QVariant qv );
    void rotationChanged( QVariant qv );
};

#endif /* DATASETCONGLYPHS_H_ */
