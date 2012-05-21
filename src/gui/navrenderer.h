/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef NAVRENDERER_H_
#define NAVRENDERER_H_

#include "../glew/include/glew.h"

#include <QtGui/QMatrix4x4>

#include "objectrenderer.h"

class DataStore;
class QGLShaderProgram;

class NavRenderer : public ObjectRenderer
{
public:
	NavRenderer( QString name );
	virtual ~NavRenderer();

	void resizeGL( int width, int height );

	void init();
	void initGL();
	void draw();

	void leftMouseDown( int x, int y );
	void leftMouseDrag( int x, int y );

	void adjustRatios();

private:
	void drawAxial();
	void drawCoronal();
	void drawSagittal();

	void drawCrosshair();

	void initShader();
    void setupTextures();
    void setShaderVars();

    void initGeometry();

	QString m_name;

	float m_ratio;

	QMatrix4x4 m_mvpMatrix;

    GLuint *vboIds;

    QGLShaderProgram *m_program;
    QGLShaderProgram *m_crosshairProgram;

    float m_x;
    float m_y;
    float m_z;
    float m_xb;
    float m_yb;
    float m_zb;

    int m_width;
    int m_height;
};

#endif /* SCENERENDERER_H_ */
