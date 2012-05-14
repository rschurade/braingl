/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SCENERENDERER_H_
#define SCENERENDERER_H_

#include "../glew/include/glew.h"

#include <QtGui/QMatrix4x4>

class ArcBall;
class DataStore;
class SliceRenderer;

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

private:
	ArcBall* m_arcBall;
	DataStore* m_dataStore;
	SliceRenderer* m_sliceRenderer;

	float m_ratio;

	QMatrix4x4 m_lastRot;
	QMatrix4x4 m_thisRot;

	QMatrix4x4 m_mvpMatrix;
};

#endif /* SCENERENDERER_H_ */
