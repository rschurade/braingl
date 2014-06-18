/*
 * objectrenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef OBJECTRENDERER_H_
#define OBJECTRENDERER_H_

#include <QVector3D>
#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLFunctions_3_3_Core>

#include <initializer_list>

class PropertyGroup;

class ObjectRenderer : public QOpenGLFunctions_3_3_Core, public QObject
{
public:
	ObjectRenderer();
	virtual ~ObjectRenderer();

	virtual void init();
	virtual void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props );

protected:
    QString createSettingsString( std::initializer_list<QVariant> settings );
    virtual void initGeometry( PropertyGroup& props );
    virtual void setShaderVars( PropertyGroup& props );

    void unproject( float x, float y, float z, QMatrix4x4& mvMatrix, QMatrix4x4& pMatrix, QVector4D& viewport, float& xOut, float& yOut, float& zOut );
    bool project( float x, float y, float z, QMatrix4x4& mvMatrix, QMatrix4x4& pMatrix, QVector4D& viewport, float& xOut, float& yOut, float& zOut );

    QString m_previousSettings;
};

#endif /* OBJECTRENDERER_H_ */
