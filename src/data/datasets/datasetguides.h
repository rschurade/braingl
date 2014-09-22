/*
 * datasetguides.h
 *
 *  Created on: 08.07.2014
 *      Author: Ralph
 */

#ifndef DATASETGUIDES_H_
#define DATASETGUIDES_H_

#include "dataset.h"

class DatasetGuides : public Dataset
{
    Q_OBJECT

public:
    DatasetGuides();
    virtual ~DatasetGuides();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

private:
    bool m_dirty;

    GLuint vbo0;
    GLuint vbo1;

    int m_vertCountAxial;
    int m_vertCountCoronal;
    int m_vertCountSagittal;

    QColor m_color;

    float m_x;
    float m_y;
    float m_z;

    void initGeometry();

private slots:
    void globalChanged();

    void addGlyph( std::vector<float> &verts, std::vector<float> &colors, float x1, float y1, float z1, float x2, float y2, float z2 );
};

#endif /* DATASETGUIDES_H_ */
