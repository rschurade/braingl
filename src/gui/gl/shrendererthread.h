/*
 * shrendererthread.h
 *
 * Created on: 27.12.2012
 * @author Ralph Schurade
 */

#ifndef SHRENDERERTHREAD_H_
#define SHRENDERERTHREAD_H_

#include "../../data/properties/propertygroup.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QDebug>
#include <QThread>
#include <QVector>
#include <QMatrix4x4>

class SHRendererThread : public QThread
{
    Q_OBJECT

public:
    SHRendererThread( std::vector<ColumnVector>* data, QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, PropertyGroup& props );
    virtual ~SHRendererThread();

    std::vector<float>* getVerts() { return m_verts; };
    std::vector<float>* getNormals() { return m_normals; };
    std::vector<float>* getColors() { return m_colors; };
    std::vector<unsigned int>* getIndices() { return m_indices; };

private:
    void run();

    std::vector<ColumnVector>* m_data;

    float m_x;
    float m_y;
    float m_z;
    float m_nx;
    float m_ny;
    float m_nz;
    float m_dx;
    float m_dy;
    float m_dz;
    float m_ax;
    float m_ay;
    float m_az;

    int m_xi;
    int m_yi;
    int m_zi;

    int m_lod;
    int m_order;
    int m_orient;
    bool m_scaling;
    bool m_hideNegative;
    float m_offset;

    QMatrix4x4 m_pMatrix;
    QMatrix4x4 m_mvMatrix;

    std::vector<float>* m_verts;
    std::vector<float>* m_normals;
    std::vector<float>* m_colors;
    std::vector<unsigned int>* m_indices;

signals:
    void finished( bool success );
};

#endif /* SHRENDERERTHREAD_H_ */
