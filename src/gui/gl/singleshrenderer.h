/*
 * singleshrenderer.h
 *
 * Created on: 12.07.2012
 * @author Ralph Schurade
 */

#ifndef SINGLESHRENDERER_H_
#define SINGLESHRENDERER_H_

#include "../../thirdparty/newmat10/newmat.h"

#include <QMatrix4x4>

#include <initializer_list>

class ArcBall;
class DatasetDWI;

class SingleSHRenderer
{
public:
    SingleSHRenderer();
    virtual ~SingleSHRenderer();

    void resizeGL( int width, int height );

    void init();

    void initGL();
    void draw();

    void leftMouseDown( int x, int y );
    void leftMouseDrag( int x, int y );

private:
    void setupTextures();
    void setShaderVars();

    void initGeometry();

    void calcMVPMatrix();

    QString createSettingsString( std::initializer_list<QVariant> settings );

    GLuint *vboIds;

    int m_width;
    int m_height;
    float m_ratio;

    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_lastRot;
    QMatrix4x4 m_thisRot;

    float m_dx;
    float m_dy;
    float m_dz;
    int m_nx;
    int m_ny;
    int m_nz;

    int m_tris;

    ArcBall* m_arcBall;

    DatasetDWI* m_dataset;

    QString m_previousSettings;
};

#endif /* SINGLESHRENDERER_H_ */
