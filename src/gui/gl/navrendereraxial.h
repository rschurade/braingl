/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef NAVRENDERERAXIAL_H_
#define NAVRENDERERAXIAL_H_

#include "../../thirdparty/glew/include/glew.h"


#include <QtGui/QMatrix4x4>

#include "navrenderer.h"

class QGLShaderProgram;

class NavRendererAxial : public NavRenderer
{
public:
	NavRendererAxial( QString name );
	virtual ~NavRendererAxial();

	void draw();

	void leftMouseDown( int x, int y );

	void adjustRatios();

private:
    void initGeometry();
};

#endif /* SCENERENDERER_H_ */
