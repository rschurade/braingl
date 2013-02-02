/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef COMBINEDNAVRENDERER_H_
#define COMBINEDNAVRENDERER_H_

#include "objectrenderer.h"

#include <QtGui/QMatrix4x4>

class DataStore;
class QGLShaderProgram;

class CombinedNavRenderer : public ObjectRenderer
{
public:
	CombinedNavRenderer( QAbstractItemModel* dataModel, QString name );
	virtual ~CombinedNavRenderer();

	void resizeGL( int width, int height );

	void init();

	void initGL();
	void draw();

	void leftMouseDown( int x, int y );
	void leftMouseDrag( int x, int y );

	void adjustRatios();

private:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

    QAbstractItemModel* m_dataModel;
	QString m_name;

	float m_ratio;

	QMatrix4x4 m_mvpMatrix;

    GLuint *vboIds;

    int m_x;
    int m_y;
    int m_z;
    float m_dx;
    float m_dy;
    float m_dz;
    int m_nx;
    int m_ny;
    int m_nz;

    int m_width;
    int m_height;
};

#endif /* SCENERENDERER_H_ */
