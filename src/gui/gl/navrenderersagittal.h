/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef NAVRENDERERSAGITTAL_H_
#define NAVRENDERERSAGITTAL_H_

#include "../../thirdparty/glew/include/glew.h"

#include <QtGui/QMatrix4x4>

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
};

#endif /* SCENERENDERER_H_ */
