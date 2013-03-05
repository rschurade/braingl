/*
 * shrendererthread.h
 *
 *  Created on: 27.12.2012
 *      Author: Ralph Schurade
 */

#ifndef SHRENDERERTHREAD_H_
#define SHRENDERERTHREAD_H_

#include "../../thirdparty/newmat10/newmat.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtGui/QMatrix4x4>

class SHRendererThread : public QThread
{
public:
    SHRendererThread( int id, QVector<ColumnVector>* data, int   m_nx,   int m_ny,   int m_nz,
                                                     float m_dx, float m_dy, float m_dz,
                                                     int   xi,     int yi,       int zi,
                                                     int lod, int order, int orient, bool scaling,
                                                     QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix );
    virtual ~SHRendererThread();

    QVector<float>* getVerts();

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

    QVector<float>* m_verts;
};

#endif /* SHRENDERERTHREAD_H_ */
