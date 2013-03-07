/*
 * sphererenderer.h
 *
 *  Created on: 07.03.2013
 *      Author: Ralph Schurade
 */

#ifndef SPHERERENDERER_H_
#define SPHERERENDERER_H_

#include "objectrenderer.h"

#include <QtGui/QColor>

class SphereRenderer : public ObjectRenderer
{
public:
	SphereRenderer();
	virtual ~SphereRenderer();

	void init();

	void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix );

	void updateGeometry( float x, float y, float z, float dx, float dy, float dz, QColor color );

	unsigned int getPickId();

protected:
    void setShaderVars();
    void initGeometry();

private:
	GLuint *vboIds;

	float m_x;
	float m_y;
	float m_z;
	float m_dx;
    float m_dy;
    float m_dz;
	QColor m_color;

	unsigned int m_pickId;
};

#endif /* SLICERENDERER_H_ */
