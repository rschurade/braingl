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
    TubeRendererThread( QVector< QVector< float > >* data, int id );
    virtual ~TubeRendererThread();

    QVector<float>* getVerts();
    QVector<QVector3D>* getGlobalColors();

private:
    void run();

    QVector< QVector< float > >* m_data;
    int m_id;

    QVector<float>* m_verts;
    QVector<QVector3D>* m_globalColors;

};

#endif /* TUBERENDERERTHREAD_H_ */
