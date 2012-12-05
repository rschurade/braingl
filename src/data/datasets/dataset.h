/*
 * dataset.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

#include "../../thirdparty/glew/include/glew.h"

#include <limits>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <QtGui/QMatrix4x4>

#include "../enums.h"

class DataStore;

class Dataset
{
public:
    Dataset( QDir fileName, FN_DATASET_TYPE type );
    virtual ~Dataset();

    QVariant getProperty( QString name );
    void setProperty( QString name, QVariant value );
    QHash<QString, QVariant> getProperties();
    void setProperties( QHash<QString, QVariant> props );


    GLuint getTextureGLuint();

    virtual void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore ) = 0;

    virtual QString getValueAsString( int x, int y, int z ) = 0;

protected:
    virtual void createTexture() = 0;

    QHash<QString, QVariant> m_properties;

    GLuint m_textureGLuint;
};

#endif /* DATASET_H_ */
