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
    MeshThread( std::vector<float>* vertices, std::vector<unsigned int>* triangles, unsigned int numTris, unsigned int bufferSize, int id );
    virtual ~MeshThread();

    std::vector<QVector3D>* getTriNormals();

private:
    void run();

    std::vector<float>* m_vertices;
    std::vector<unsigned int>* m_triangles;
    std::vector<QVector3D> m_triNormals;
    unsigned int m_numTris;
    unsigned int m_bufferSize;
    int m_id;
};

#endif /* MESHTHREAD_H_ */
