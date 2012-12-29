/*
 * meshthread.h
 *
 *  Created on: 29.12.2012
 *      Author: Ralph
 */

#ifndef MESHTHREAD_H_
#define MESHTHREAD_H_

#include <QThread>
#include <QtCore/QVector>
#include <QtGui/QVector3D>

class MeshThread : public QThread
{
public:
    MeshThread( QVector<float>* vertices, QVector<int>* triangles, int numTris, int id );
    virtual ~MeshThread();

    QVector<QVector3D>* getTriNormals();

private:
    void run();

    QVector<float>* m_vertices;
    QVector<int>* m_triangles;
    QVector<QVector3D> m_triNormals;
    int m_numTris;
    int m_id;
};

#endif /* MESHTHREAD_H_ */
