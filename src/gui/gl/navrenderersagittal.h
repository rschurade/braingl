/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph Schurade
 */

#ifndef NAVRENDERERSAGITTAL_H_
#define NAVRENDERERSAGITTAL_H_

#include "navrenderer.h"

#include <QMatrix4x4>

class QGLShaderProgram;

class NavRendererSagittal : public NavRenderer
{
public:
	NavRendererSagittal(  QAbstractItemModel* dataModel, QString name );
	virtual ~NavRendererSagittal();

	void draw();

	void leftMouseDown( int x, int y );

	void adjustRatios();

private:
    void initGeometry();
};

#endif /* SCENERENDERER_H_ */
