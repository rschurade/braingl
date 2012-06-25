/*
 * slicerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SLICERENDERER_H_
#define SLICERENDERER_H_

#include "../../thirdparty/glew/include/glew.h"

#include "objectrenderer.h"

class QGLShaderProgram;

class SliceRenderer : public ObjectRenderer
{
public:
	SliceRenderer();
	virtual ~SliceRenderer();

	void init();

	void draw( QMatrix4x4 mvp_matrix );

protected:
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

	int m_xOld;
    int m_yOld;
    int m_zOld;
    int m_xbOld;
    int m_ybOld;
    int m_zbOld;

	GLuint *vboIds;
};

#endif /* SLICERENDERER_H_ */
