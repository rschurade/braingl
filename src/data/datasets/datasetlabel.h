/*
 * datasetlabel.h
 *
 *  Created on: 03.08.2014
 *      Author: Ralph Schurade
 */
#ifndef DATASETLABEL_H_
#define DATASETLABEL_H_

#include "dataset.h"

class DatasetLabel : public Dataset
{
    Q_OBJECT

public:
    DatasetLabel();
    virtual ~DatasetLabel();

    void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target );

    bool rightMouseDrag( int pickId, QVector3D dir, Qt::KeyboardModifiers modifiers, QString target );

private:
    int m_handle0;
};

#endif /* DATASETLABEL_H_ */
