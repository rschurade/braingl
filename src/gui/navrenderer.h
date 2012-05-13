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

class DataStore;
class QGLShaderProgram;

class NavRenderer
{
public:
	NavRenderer( DataStore* dataStore, QString name );
	virtual ~NavRenderer();

	void resizeGL( int width, int height );

	void initGL();
	void draw();

	void leftMouseDown( int x, int y );
	void leftMouseDrag( int x, int y );

private:
	void drawAxial();
	void drawCoronal();
	void drawSagittal();

	void initShader();
    void setupTextures();
    void setShaderVars();

    void initGeometry();

	DataStore* m_dataStore;
	QString m_name;

	float m_ratio;

	QMatrix4x4 m_mvpMatrix;

    GLuint *vboIds;

    QGLShaderProgram *m_program;

    float m_x;
    float m_y;
    float m_z;
    float m_xb;
    float m_yb;
    float m_zb;
};

#endif /* SCENERENDERER_H_ */
