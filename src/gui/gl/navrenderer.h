/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef NAVRENDERER_H_
#define NAVRENDERER_H_

#include "../../glew/include/glew.h"

#include <QtGui/QMatrix4x4>

#include "objectrenderer.h"

class QGLShaderProgram;

class NavRenderer : public ObjectRenderer
{
public:
	NavRenderer( QString name );
	virtual ~NavRenderer();

	void resizeGL( int width, int height );

	void init();
	void initGL();
	virtual void draw()=0;

	virtual void leftMouseDown( int x, int y )=0;
	virtual void leftMouseDrag( int x, int y );

	virtual void adjustRatios()=0;

protected:
	void initShader();
    void setupTextures();
    void setShaderVars();

    virtual void initGeometry()=0;

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
    float m_xd;
    float m_yd;
    float m_zd;
    int m_xOld;
    int m_yOld;
    int m_zOld;

    int m_width;
    int m_height;

    int m_xoff;
    int m_yoff;
};

#endif /* SCENERENDERER_H_ */
