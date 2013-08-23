/*
 * loaderfreesurfer.h
 *
 *  Created on: Apr 22, 2013
 *      Author: boettgerj
 */

#ifndef LOADERFREESURFER_H_
#define LOADERFREESURFER_H_

#include <QString>
#include <QVector>

class LoaderFreesurfer
{
public:
    LoaderFreesurfer();
    virtual ~LoaderFreesurfer();

    bool loadASC( QString fn );

    QVector<float> getPoints();
    QVector<int> getTriangles();

private:
    QVector<float> m_points;
    QVector<int> m_triangles;
};

#endif /* LOADERFREESURFER_H_ */
