/*
 * dataset.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

#include "../../thirdparty/glew/include/glew.h"

#include "../enums.h"
#include "../properties/propertygroup.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QMatrix4x4>

#include <limits>

class QAbstractItemModel;

class Dataset
{
public:
    Dataset( QDir fileName, Fn::DatasetType type, QAbstractItemModel* roiModel = 0 );
    virtual ~Dataset();

    PropertyGroup* properties();

    GLuint getTextureGLuint();

    virtual void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, QAbstractItemModel* model );

    virtual QString getValueAsString( int x, int y, int z );

    void setRoiModel( QAbstractItemModel* roiModel );
    QAbstractItemModel* getRoiModel();

protected:
    virtual void createTexture();

    QAbstractItemModel* m_roiModel;

    PropertyGroup m_properties;

    GLuint m_textureGLuint;
};

#endif /* DATASET_H_ */
