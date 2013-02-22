/*
 * colormaprenderer.h
 *
 *  Created on: Feb 18, 2013
 *      Author: schurade
 */

#ifndef COLORMAPRENDERER_H_
#define COLORMAPRENDERER_H_

#include "objectrenderer.h"

class QGLShaderProgram;

class ColormapRenderer : public ObjectRenderer
{
    Q_OBJECT

public:
    ColormapRenderer();
    virtual ~ColormapRenderer();

    void init();
    void draw();

private:

    void initGeometry();
    void setShaderVars();

    GLuint *vboIds;

    int m_orient;
    float m_x;
    float m_y;
    float m_dx;
    float m_dy;
    int m_textSize;

    int m_colormap;
    float m_min;
    float m_max;
    float m_selectedMin;
    float m_selectedMax;
    float m_lowerThreshold;
    float m_upperThreshold;
    QVector<QVector3D>m_labels;

public slots:
    void setX( float x );
    void setY( float y );
    void setDX( float dx );
    void setDY( float dy );
    void setTextSize( int size );

    void setColormap( int value );
    void setMin( float value );
    void setMax( float value );
    void setSelectedMin( float value );
    void setSelectedMax( float value );
    void setLowerThreshold( float value );
    void setUpperThreshold( float value );
};

#endif /* COLORMAPRENDERER_H_ */
