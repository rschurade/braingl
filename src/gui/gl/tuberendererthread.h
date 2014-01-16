/*
 * tuberendererthread.h
 *
 * Created on: 26.02.2013
 * @author Ralph Schurade
 */

#ifndef TUBERENDERERTHREAD_H_
#define TUBERENDERERTHREAD_H_

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QVector3D>

class TubeRendererThread : public QThread
{
public:
    TubeRendererThread( QVector< std::vector<float> >* data, int id );
    virtual ~TubeRendererThread();

    std::vector<float>* getVerts();
    QVector<QVector3D>* getGlobalColors();

private:
    void run();

    QVector< std::vector<float> >* m_data;
    int m_id;

    std::vector<float>* m_verts;
    QVector<QVector3D>* m_globalColors;

};

#endif /* TUBERENDERERTHREAD_H_ */
