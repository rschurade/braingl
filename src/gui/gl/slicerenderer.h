/*
 * slicerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph Schurade
 */

#ifndef SLICERENDERER_H_
#define SLICERENDERER_H_

#include "objectrenderer.h"

class QGLShaderProgram;

class SliceRenderer : public ObjectRenderer
{
public:
	SliceRenderer( QAbstractItemModel* dataModel );
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

	QAbstractItemModel* m_dataModel;

	GLuint *vboIds;

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
};

#endif /* SLICERENDERER_H_ */
