/*
 * shrendererthread.h
 *
 * Created on: 27.12.2012
 * @author Ralph Schurade
 */

#ifndef SHRENDERERTHREAD2_H_
#define SHRENDERERTHREAD2_H_

#include "../../data/properties/propertygroup.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QMatrix4x4>

class TriangleMesh2;

class SHRendererThread2 : public QThread
{
    Q_OBJECT

public:
    SHRendererThread2( int id,
                         std::vector<ColumnVector>* data,
                         QMatrix4x4 pMatrix,
                         QMatrix4x4 mvMatrix,
                         PropertyGroup& props );
    virtual ~SHRendererThread2();

    TriangleMesh2* getMesh();

private:
    void run();

    int m_id;

    std::vector<ColumnVector>* m_data;

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_mvMatrix;

    TriangleMesh2* m_mesh;

    PropertyGroup& m_props;

signals:
    void finished( TriangleMesh2* );
};

#endif /* SHRENDERERTHREAD2_H_ */
