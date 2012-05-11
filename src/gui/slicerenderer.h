/*
 * slicerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SLICERENDERER_H_
#define SLICERENDERER_H_

#include "../glew/include/glew.h"

#include "objectrenderer.h"

class QGLShaderProgram;

class SliceRenderer : public ObjectRenderer
{
public:
	SliceRenderer( DataStore* dataStore );
	virtual ~SliceRenderer();

	void init();

	void draw( QMatrix4x4 rotation, float ratio );

protected:
    void initShader();
    void setupTextures();
    void setShaderVars();

    void initGeometry();

private:
	void drawAxial();
	void drawCoronal();
	void drawSagittal();

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

};

#endif /* SLICERENDERER_H_ */
