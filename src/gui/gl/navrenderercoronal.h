/*
 * scenerenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef NAVRENDERERCORONAL_H_
#define NAVRENDERERCORONAL_H_

#include "navrenderer.h"

#include <QMatrix4x4>

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

    QMatrix4x4 m_pMatrix;
};

#endif /* SCENERENDERER_H_ */
