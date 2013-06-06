/*
 * dataset.h
 *
 * Created on: May 3, 2012
 * @author Ralph Schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

#include "GL/glew.h"

#include "../enums.h"
#include "../properties/propertygroup.h"

#include <QDebug>
#include <QDir>
#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>
#include <QMatrix4x4>

#include <limits>

class Dataset : public QObject
{
public:
    Dataset( QDir fileName, Fn::DatasetType type );
    virtual ~Dataset();

    PropertyGroup* properties( QString target = "maingl" );

    GLuint getTextureGLuint();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target ) = 0;

    virtual QString getValueAsString( int x, int y, int z );
    virtual QString getColormapShader( int num );

    virtual void mousePick( int pickId, QVector3D pos );

protected:
    virtual void createTexture();

    QHash< QString, PropertyGroup* >m_properties;

    GLuint m_textureGLuint;
};

#endif /* DATASET_H_ */
