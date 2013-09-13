/*
 * point.h
 *
 *  Created on: Sep 4, 2013
 *      Author: schurade
 */

#ifndef POINT_H_
#define POINT_H_

#include <QVector3D>
#include <QSet>

class Point
{
public:
    Point();
    Point( QVector3D pos );
    virtual ~Point();


    QVector3D position;
    int newID;
    bool toDelete;
    QSet<int>inTriangle;
};

#endif /* POINT_H_ */
