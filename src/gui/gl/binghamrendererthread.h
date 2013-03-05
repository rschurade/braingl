/*
 * BinghamRendererThread.h
 *
 *  Created on: 27.12.2012
 *      Author: Ralph Schurade
 */

#ifndef BINGHAMRENDERERTHREAD_H_
#define BINGHAMRENDERERTHREAD_H_

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtGui/QMatrix4x4>

class BinghamRendererThread : public QThread
{
public:
    BinghamRendererThread( int id, QVector<QVector<float> >* data,
            int m_nx, int m_ny, int m_nz,
            float m_dx, float m_dy, float m_dz,
            int xi, int yi, int zi,
            int lod, int order, int orient, bool scaling, int renderPeaks,
            QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix );
    virtual ~BinghamRendererThread();

    QVector<float>* getVerts();

private:
    void run();

    int m_id;
    QVector<QVector<float> >* m_data;

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
    int m_renderPeaks;

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_mvMatrix;

    QVector<float>* m_verts;
};

#endif /* BinghamRendererThread_H_ */
