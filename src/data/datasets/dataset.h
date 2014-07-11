/*
 * dataset.h
 *
 * Created on: May 3, 2012
 * @author Ralph Schurade
 */

#ifndef DATASET_H_
#define DATASET_H_

#include "../enums.h"
#include "../properties/propertygroup.h"

#include <QDebug>
#include <QDir>
#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>
#include <QMatrix4x4>

#include <QOpenGLFunctions_3_3_Core>

#include <limits>

class Dataset : public QObject
{
public:
    Dataset( QDir fileName, Fn::DatasetType type );
    virtual ~Dataset();

    PropertyGroup& properties( QString target = "maingl" );

    GLuint getTextureGLuint();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target ) = 0;

    virtual QString getValueAsString( int x, int y, int z );
    virtual QString getColormapShader( int num );

    virtual bool mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target );
    virtual bool rightMouseDrag( int pickId, QVector3D dir, Qt::KeyboardModifiers modifiers, QString target );
    void copySettings( QString target );
    void copyPropertyObject( PropertyGroup& props, QString target );


    virtual QString getSaveFilter();
    virtual QString getDefaultSuffix();

    virtual QPair<QVector3D, QVector3D>getBoundingBox();

protected:
    virtual void createTexture();
    virtual void calcBoundingBox();

    QHash< QString, PropertyGroup >m_properties;

    GLuint m_textureGLuint;
    QPair<QVector3D, QVector3D>m_boundingBox;

    bool m_resetRenderer;
};

#endif /* DATASET_H_ */
