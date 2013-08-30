/*
 * shrendererthread.h
 *
 * Created on: 27.12.2012
 * @author Ralph Schurade
 */

#ifndef SHRENDERERTHREAD2_H_
#define SHRENDERERTHREAD2_H_

#include "../../thirdparty/newmat10/newmat.h"

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QMatrix4x4>

class TriangleMesh2;

class SHRendererThread2 : public QThread
{
    Q_OBJECT

public:
    SHRendererThread2( int id, QVector<ColumnVector>* data, int   m_nx,   int m_ny,   int m_nz,
                                                     float m_dx, float m_dy, float m_dz,
                                                     int   xi,     int yi,       int zi,
                                                     int lod, int order, int orient, bool scaling,
                                                     QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix );
    virtual ~SHRendererThread2();

    TriangleMesh2* getMesh();

private:
    void run();

    int m_id;

    QVector<ColumnVector>* m_data;

    int m_nx;
    int m_ny;
    int m_nz;
    float m_dx;
    float m_dy;
    float m_dz;

    int m_xi;
    int m_yi;
    int m_zi;

    int m_lod;
    int m_order;
    int m_orient;
    bool m_scaling;

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_mvMatrix;

    TriangleMesh2* m_mesh;

signals:
    void finished( TriangleMesh2* );
};

#endif /* SHRENDERERTHREAD2_H_ */
