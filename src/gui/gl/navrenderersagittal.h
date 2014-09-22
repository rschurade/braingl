/*
 * scenerenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef NAVRENDERERSAGITTAL_H_
#define NAVRENDERERSAGITTAL_H_

#include "navrenderer.h"

class QGLShaderProgram;

class NavRendererSagittal : public NavRenderer
{
public:
	NavRendererSagittal( QString name );
	virtual ~NavRendererSagittal();

	void draw();

	void leftMouseDown( int x, int y );
	void adjustRatios();

private:
	void initGeometry();

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_pMatrixWorkaround;
};

#endif /* SCENERENDERER_H_ */
