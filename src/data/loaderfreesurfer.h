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

    std::vector<float> getPoints();
    std::vector<int> getTriangles();

private:
    std::vector<float> m_points;
    std::vector<int> m_triangles;
};

#endif /* LOADERFREESURFER_H_ */
