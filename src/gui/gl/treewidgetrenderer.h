/*
 * scenerenderer.h
 *
 * Created on: 17.09.2013
 * @author Ralph Schurade
 */

#ifndef TREEWIDGTERENDERER_H_
#define TREEWIDGTERENDERER_H_

#include "objectrenderer.h"

#include <QMatrix4x4>

class Tree;
class QGLShaderProgram;

class TreeWidgetRenderer : public ObjectRenderer
{
public:
	TreeWidgetRenderer( QString name );
	virtual ~TreeWidgetRenderer();

	void resizeGL( int width, int height );

	void init();
	void initGL();
	void draw();

	virtual void leftMouseDown( int x, int y );
	virtual void leftMouseDrag( int x, int y );
	void middleMouseDown( int x, int y );
    void middleMouseDrag( int x, int y );
	virtual void mouseWheel( int step );

private:
	void setShaderVars();

	QString m_name;

	QMatrix4x4 m_mvpMatrix;

    int m_width;
    int m_height;
};

#endif /* SCENERENDERER_H_ */
