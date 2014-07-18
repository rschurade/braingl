/*
 * datasetisoline.h
 *
 *  Created on: 08.05.2014
 *      Author: Ralph
 */

#ifndef DATASETISOLINE_H_
#define DATASETISOLINE_H_

#include "dataset.h"

class DatasetScalar;

class DatasetIsoline : public Dataset
{
    Q_OBJECT

public:
    DatasetIsoline( DatasetScalar* ds );
    virtual ~DatasetIsoline();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

    std::vector<float>* getData();
    QString getSaveFilter();
    QString getDefaultSuffix();

private:
    std::vector<float> m_scalarField;

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

    std::vector<float>extractAnatomyAxial( float z );
    std::vector<float>extractAnatomySagittal( float x );
    std::vector<float>extractAnatomyCoronal( float y );

    int getId( int x, int y, int z );

private slots:
    void isoValueChanged();
    void globalChanged();

    void addGlyph( std::vector<float> &verts, std::vector<float> &colors, float x1, float y1, float z1, float x2, float y2, float z2 );

};

#endif /* DATASETISOLINE_H_ */
