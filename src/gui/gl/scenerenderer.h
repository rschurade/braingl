/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SCENERENDERER_H_
#define SCENERENDERER_H_

#include "../../data/enums.h"

#include <QtGui/QMatrix4x4>
#include <QtCore/QModelIndex>

class ArcBall;
class DataStore;
class SliceRenderer;
class QAbstractItemModel;
class QItemSelectionModel;

class SceneRenderer
{
public:
	SceneRenderer( QAbstractItemModel* dataModel, QAbstractItemModel* globalModel, QAbstractItemModel* roiModel, QItemSelectionModel* roiSelectionModel );
	virtual ~SceneRenderer();

	void resizeGL( int width, int height );

	void initGL();
	void draw();

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

	QVector3D mapMouse2World( int x, int y, int dir );

	ArcBall* m_arcBall;
	SliceRenderer* m_sliceRenderer;

	QAbstractItemModel* m_dataModel;
	QAbstractItemModel* m_globalModel;
	QAbstractItemModel* m_roiModel;
	QItemSelectionModel* m_roiSelectionModel;

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

	int m_pickXold;
	int m_pickYold;
};

#endif /* SCENERENDERER_H_ */
