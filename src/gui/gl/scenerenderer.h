/*
 * scenerenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef SCENERENDERER_H_
#define SCENERENDERER_H_

#include "GL/glew.h"

#include "../../data/enums.h"

#include <QVector2D>
#include <QMatrix4x4>
#include <QModelIndex>

class ArcBall;
class DataStore;
class SliceRenderer;
class QItemSelectionModel;

class SceneRenderer
{
public:
	SceneRenderer( QItemSelectionModel* roiSelectionModel );
	virtual ~SceneRenderer();

	void resizeGL( int width, int height );

	void initGL();
	void draw();
	void renderMerge();
	QImage* screenshot();

	void leftMouseDown( int x, int y );
	void leftMouseDrag( int x, int y );
	void middleMouseDown( int x, int y );
	void middleMouseDrag( int x, int y );
	void rightMouseDown( int x, int y );
	void rightMouseDrag( int x, int y );
	void mouseWheel( int step );
	void setView( Fn::Orient view );

	void calcMVPMatrix();

private:
	void renderDatasets();
	void renderRois();

	void renderPick();

	QVector3D mapMouse2World( int x, int y, int dir );
	QVector2D mapWorld2Mouse( float x, float y, float z );
	QVector3D mapMouse2World( float x, float y );

	ArcBall* m_arcBall;
	SliceRenderer* m_sliceRenderer;

	QItemSelectionModel* m_roiSelectionModel;

	GLuint *vboIds;

	bool skipDraw;

	int m_boundingbox;

	float m_nx;
	float m_ny;
	float m_nz;

	int m_width;
	int m_height;
	float m_ratio;

	QMatrix4x4 m_mvMatrix;
	QMatrix4x4 m_pMatrix;
	QMatrix4x4 m_mvpMatrix;
	QMatrix4x4 m_mvMatrixInverse;

	int m_picked;
	QVector2D m_rightMouseDown;
	QVector2D m_pickOld;
	int m_sliceXPosAtPick;
	int m_sliceYPosAtPick;
	int m_sliceZPosAtPick;
};

#endif /* SCENERENDERER_H_ */
