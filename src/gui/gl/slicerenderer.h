/*
 * slicerenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef SLICERENDERER_H_
#define SLICERENDERER_H_

#include "objectrenderer.h"

class QGLShaderProgram;

class SliceRenderer : public ObjectRenderer
{
public:
	SliceRenderer();
	virtual ~SliceRenderer();

	void init();

	void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode );

protected:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

private:
	void drawAxial();
	void drawCoronal();
	void drawSagittal();

	GLuint *vboIds;

	float m_x;
	float m_y;
	float m_z;
	float m_nx;
	float m_ny;
	float m_nz;
	float m_dx;
	float m_dy;
	float m_dz;

	int m_xOld;
    int m_yOld;
    int m_zOld;
    int m_xbOld;
    int m_ybOld;
    int m_zbOld;
};

#endif /* SLICERENDERER_H_ */
