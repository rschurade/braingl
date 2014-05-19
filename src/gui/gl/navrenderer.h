/*
 * scenerenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef NAVRENDERER_H_
#define NAVRENDERER_H_

#include <QMatrix4x4>

class PropertyGroup;
class QGLShaderProgram;

class NavRenderer
{
public:
	NavRenderer( QString name );
	virtual ~NavRenderer();

	void resizeGL( int width, int height );

	void init();
	void initGL();
	virtual void draw()=0;
	void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props ) {};

	virtual void leftMouseDown( int x, int y )=0;
	virtual void leftMouseDrag( int x, int y );
	void middleMouseDown( int x, int y );
    void middleMouseDrag( int x, int y );
	virtual void mouseWheel( int step );

	virtual void adjustRatios()=0;

protected:
    void setupTextures();
    void setShaderVars();

    virtual void initGeometry()=0;

	QString m_name;

	float m_ratio;

	QMatrix4x4 m_mvpMatrix;

    GLuint *vboIds;

    float m_x;
    float m_y;
    float m_z;
    float m_nx;
    float m_ny;
    float m_nz;
    float m_dx;
    float m_dy;
    float m_dz;
    int m_xOld;
    int m_yOld;
    int m_zOld;

    int m_width;
    int m_height;

    int m_xoff;
    int m_yoff;

    int m_zoom;
    int m_middleDownX;
    int m_middleDownY;
    int m_moveX;
    int m_moveY;
    int m_moveXOld;
    int m_moveYOld;
};

#endif /* SCENERENDERER_H_ */
