/*
 * fiberrendererthread.h
 *
 * Created on: 28.12.2012
 * @author Ralph Schurade
 */

#ifndef FIBERRENDERERTHREAD_H_
#define FIBERRENDERERTHREAD_H_

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QColor>

class FiberRendererThread : public QThread
{
public:
    FiberRendererThread( QVector< QVector< float > >* data, int id );
    virtual ~FiberRendererThread();

    QVector<float>* getVerts();
    QVector<QVector3D>* getGlobalColors();

private:
    void run();

    QVector< QVector< float > >* m_data;
    int m_id;

    QVector<float>* m_verts;
    QVector<QVector3D>*m_globalColors;

};

#endif /* FIBERRENDERERTHREAD_H_ */
