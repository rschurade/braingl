/*
 * scenerenderer.h
 *
 * Created on: 17.09.2013
 * @author Ralph Schurade
 */

#ifndef TREERENDERER_H_
#define TREERENDERER_H_

#include "objectrenderer.h"

#include <QMatrix4x4>

class Tree;
class QGLShaderProgram;

class TreeRenderer : public ObjectRenderer
{
public:
	TreeRenderer( QString name, Tree* tree );
	virtual ~TreeRenderer();

	void resizeGL( int width, int height );

	void init();
	void initGL();
	void draw( QMatrix4x4 mvpMatrix );

	virtual void leftMouseDown( int x, int y );
	virtual void leftMouseDrag( int x, int y );
	void middleMouseDown( int x, int y );
    void middleMouseDrag( int x, int y );
	virtual void mouseWheel( int step );

	void update();

private:
    void setShaderVars();
    void initGeometry();

    void initGeometryRec( Tree* tree, int left, int right );

	QString m_name;
	Tree* m_tree;

    GLuint *vboIds;

    int m_width;
    int m_height;

    QVector<float>m_verts;
    QVector<float>m_colors;


    bool m_dirty;
};

#endif /* SCENERENDERER_H_ */
