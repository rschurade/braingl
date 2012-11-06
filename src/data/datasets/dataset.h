/*
 * dataset.h
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

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

    virtual void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse, DataStore* datastore ) = 0;

protected:
    QHash< QString, QVariant >m_properties;
};

#endif /* DATASET_H_ */
