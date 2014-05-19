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

    void setIso1Verts( std::vector<float> verts );
    void setIso2Verts( std::vector<float> verts );

    void setVectorVerts( std::vector<float> verts, std::vector<float> colors );

     void setIso1Color( QColor color );
     void setIso2Color( QColor color );

     void setBoundingBox( int x, int y );
     void setStippleWidth( int width );
private:
    void setShaderVars();

    void drawIso1( QMatrix4x4 mvpMatrix );
    void drawIso2( QMatrix4x4 mvpMatrix );
    void drawVectors( QMatrix4x4 mvpMatrix );

    QString m_name;

    GLuint vbo0;
    GLuint vbo1;
    GLuint vbo2;
    GLuint vbo3;
    GLuint vbo4;
    GLuint vbo5;

    std::vector<float>m_iso1Verts;
    std::vector<float>m_iso2Verts;
    std::vector<float>m_vectorVerts;
    std::vector<float>m_vectorColors;
    QColor m_iso1Color;
    QColor m_iso2Color;


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

    bool m_iso1VertsDirty;
    bool m_iso2VertsDirty;
    bool m_iso1ColorDirty;
    bool m_iso2ColorDirty;
    bool m_vectorsDirty;

    int m_boundingBoxX;
    int m_boundingBoxY;

    int m_stippleWidth;
};

#endif /* FIBERSTIPPLINGRENDERER_H_ */
