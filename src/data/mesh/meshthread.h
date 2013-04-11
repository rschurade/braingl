/*
 * meshthread.h
 *
 * Created on: 29.12.2012
 * @author Ralph Schurade
 */

#ifndef MESHTHREAD_H_
#define MESHTHREAD_H_

#include <QThread>
#include <QVector>
#include <QVector3D>

class MeshThread : public QThread
{
public:
    MeshThread( QVector<float>* vertices, QVector<int>* triangles, int numTris, int bufferSize, int id );
    virtual ~MeshThread();

    QVector<QVector3D>* getTriNormals();

private:
    void run();

    QVector<float>* m_vertices;
    QVector<int>* m_triangles;
    QVector<QVector3D> m_triNormals;
    int m_numTris;
    int m_bufferSize;
    int m_id;
};

#endif /* MESHTHREAD_H_ */
