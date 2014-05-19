/*
 * objectrenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef OBJECTRENDERER_H_
#define OBJECTRENDERER_H_

#include "GL/glew.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QObject>

#include <initializer_list>

class PropertyGroup;

class ObjectRenderer : public QObject
{
public:
	ObjectRenderer();
	virtual ~ObjectRenderer();

	virtual void init() = 0;
	virtual void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props ) = 0;

protected:
    QString createSettingsString( std::initializer_list<QVariant> settings );
    virtual void initGeometry( PropertyGroup& props ) = 0;
    virtual void setShaderVars( PropertyGroup& props ) = 0;

    QString m_previousSettings;
};

#endif /* OBJECTRENDERER_H_ */
