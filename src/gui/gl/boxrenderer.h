/*
 * slicerenderer.h
 *
 *  Created on: 04.02.2013
 *      Author: Ralph
 */

#ifndef BOXRENDERER_H_
#define BOXRENDERER_H_

#include "objectrenderer.h"

#include <QtGui/QColor>

class QGLShaderProgram;

class BoxRenderer : public ObjectRenderer
{
public:
	BoxRenderer();
	virtual ~BoxRenderer();

	void init();

	void draw( QMatrix4x4 mvp_matrix );

	void updateGeometry( float x, float y, float z, float dx, float dy, float dz, QColor color );

	unsigned int getPickId();

protected:
    void setShaderVars();
    void initGeometry();

private:
	GLuint *vboIds;

	QColor m_color;

	unsigned int m_pickId;
};

#endif /* SLICERENDERER_H_ */
