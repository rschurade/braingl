/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SCENERENDERER_H_
#define SCENERENDERER_H_

#include "../../thirdparty/glew/include/glew.h"

#include <QtGui/QMatrix4x4>

class ArcBall;
class DataStore;
class SliceRenderer;
class SHRenderer;
class TensorRenderer;

class SceneRenderer
{
public:
	SceneRenderer( DataStore* dataStore );
	virtual ~SceneRenderer();

	void resizeGL( int width, int height );

	void initGL();
	void draw();

	void leftMouseDown( int x, int y );
	void leftMouseDrag( int x, int y );
	void middleMouseDown( int x, int y );
	void middleMouseDrag( int x, int y );
	void mouseWheel( int step );
	void setView( int view );

	void calcMVPMatrix();

private:
	ArcBall* m_arcBall;
	DataStore* m_dataStore;
	SliceRenderer* m_sliceRenderer;
	SHRenderer* m_shRenderer;
	TensorRenderer* m_tensorRenderer;

	int m_boundingbox;

	float m_datasetSizeX;
	float m_datasetSizeY;
	float m_datasetSizeZ;

	int m_width;
	int m_height;
	float m_ratio;

	QMatrix4x4 m_lastRot;
	QMatrix4x4 m_thisRot;

	QMatrix4x4 m_mvpMatrix;

	int m_zoom;
	int m_middleDownX;
	int m_middleDownY;
	int m_moveX;
	int m_moveY;
	int m_moveXOld;
    int m_moveYOld;

};

#endif /* SCENERENDERER_H_ */
