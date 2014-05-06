/*
 * fiberstipplingrenderer.h
 *
 *  Created on: May 6, 2014
 *      Author: schurade
 */

#ifndef FIBERSTIPPLINGRENDERER_H_
#define FIBERSTIPPLINGRENDERER_H_

#include "objectrenderer.h"

#include <QMatrix4x4>

class QGLShaderProgram;

class FiberStipplingRenderer : public ObjectRenderer
{
public:
    FiberStipplingRenderer( QString name );
    virtual ~FiberStipplingRenderer();

    void resizeGL( int width, int height );

    void init();
    void initGL();
    void draw();

    virtual void leftMouseDown( int x, int y );
    virtual void leftMouseDrag( int x, int y );
    void middleMouseDown( int x, int y );
    void middleMouseDrag( int x, int y );
    virtual void mouseWheel( int step );

    float getMoveX() { return m_moveX; };
    int getZoom() { return m_zoom; };

    void setIso1( std::vector<float> verts );

private:
    void setShaderVars();

    void drawIso1( QMatrix4x4 mvpMatrix );

    QString m_name;

    GLuint *vboIds;

    QMatrix4x4 m_mvpMatrix;

    int m_width;
    int m_height;
    int m_zoom;
    float m_moveX;
    float m_moveY;
    float m_moveXOld;
    float m_moveYOld;
    int m_middleDownX;
    int m_middleDownY;

    std::vector<float> m_iso1Verts;
    std::vector<float>m_colors;
};

#endif /* FIBERSTIPPLINGRENDERER_H_ */
