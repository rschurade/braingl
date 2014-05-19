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
    FiberRendererThread( std::vector< std::vector<float> >* data, int id );
    virtual ~FiberRendererThread();

    std::vector<float>* getVerts();
    std::vector<QVector3D>* getGlobalColors();

private:
    void run();

    std::vector< std::vector<float> >* m_data;
    int m_id;

    std::vector<float>* m_verts;
    std::vector<QVector3D>*m_globalColors;

};

#endif /* FIBERRENDERERTHREAD_H_ */
