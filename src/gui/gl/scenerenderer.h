/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SCENERENDERER_H_
#define SCENERENDERER_H_

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
	void middleMouseDown( int x, int y );
	void middleMouseDrag( int x, int y );
	void mouseWheel( int step );
	void setView( int view );

	void calcMVPMatrix();

private:
	ArcBall* m_arcBall;
	DataStore* m_dataStore;
	SliceRenderer* m_sliceRenderer;

	int m_boundingbox;

	float m_datasetSizeX;
	float m_datasetSizeY;
	float m_datasetSizeZ;

	int m_width;
	int m_height;
	float m_ratio;

	QMatrix4x4 m_mvMatrix;

	QMatrix4x4 m_mvpMatrix;
	QMatrix4x4 m_mvMatrixInverse;
};

#endif /* SCENERENDERER_H_ */
