/*
 * BinghamRendererThread.h
 *
 *  Created on: 27.12.2012
 *      Author: Ralph
 */

#ifndef BINGHAMRENDERERTHREAD_H_
#define BINGHAMRENDERERTHREAD_H_

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QVector>

class BinghamRendererThread : public QThread
{
public:
    BinghamRendererThread( QVector<QVector<float> >* data,
            int m_nx, int m_ny, int m_nz,
            float m_dx, float m_dy, float m_dz,
            int xi, int yi, int zi,
            QVector<int> visibleArea, int lod, int order, int orient, bool scaling, int id );
    virtual ~BinghamRendererThread();

    QVector<float>* getVerts();

private:
    void run();

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

    QVector<int> m_visibleArea;
    int m_lod;
    int m_order;
    int m_orient;
    bool m_scaling;
    int m_id;

    QVector<float>* m_verts;
};

#endif /* BinghamRendererThread_H_ */
