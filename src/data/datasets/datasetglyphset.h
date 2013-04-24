/*
 * datasetglyphset.h
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#include "datasetsurfaceset.h"

#include "../../gui/gl/pointglyphrenderer.h"

#include <qstring.h>

#ifndef DATASETGLYPHSET_H_
#define DATASETGLYPHSET_H_

class DatasetGlyphset: public DatasetSurfaceset
{
public:
    DatasetGlyphset( QString filename );
    virtual ~DatasetGlyphset();
    void readConnectivity( QString filename );
    void setMinthresh( float mt );

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode );

    float* getCons();
    void setProperties();
    int consNumber;
private:
    float minthresh;
    float** conn; //square connectivity matrix
    int n;  //number of vertices, has to match size of matrix

    float* consArray;
    PointGlyphRenderer* m_prenderer;
    int prevGeo, prevGlyph, prevCol, prevGlyphstyle;
};

#endif /* DATASETGLYPHSET_H_ */
