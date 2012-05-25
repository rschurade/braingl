/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef NAVRENDERERCORONAL_H_
#define NAVRENDERERCORONAL_H_

#include "../../glew/include/glew.h"

#include <QtGui/QMatrix4x4>

#include "navrenderer.h"

class QGLShaderProgram;

class NavRendererCoronal : public NavRenderer
{
public:
	NavRendererCoronal( QString name );
	virtual ~NavRendererCoronal();

	void draw();

	void leftMouseDown( int x, int y );

	void adjustRatios();

private:
    void initGeometry();
};

#endif /* SCENERENDERER_H_ */
