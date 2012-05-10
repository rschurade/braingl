/*
 * slicerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SLICERENDERER_H_
#define SLICERENDERER_H_

#include "../glew/include/glew.h"

#include <QtGui/QMatrix4x4>

#include "objectrenderer.h"

class DataStore;
class QGLShaderProgram;

class SliceRenderer : public ObjectRenderer
{
public:
	SliceRenderer( DataStore* dataStore );
	virtual ~SliceRenderer();

	void init();
	void initShader();
	void initGeometry();
	void setShaderVars();
	void draw( QMatrix4x4 rotation );

private:

	void renderAxial();
	void renderCoronal();
	void renderSagittal();

	float m_x;
	float m_y;
	float m_z;
	float m_xb;
	float m_yb;
	float m_zb;
	float m_xc;
	float m_yc;
	float m_zc;

	GLuint *vboIds;

	QGLShaderProgram *m_program;

	QMatrix4x4 projection;
};

#endif /* SLICERENDERER_H_ */
