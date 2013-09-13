/*
 * triangle.h
 *
 *  Created on: Sep 4, 2013
 *      Author: schurade
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

class Triangle
{
public:
    Triangle();
    Triangle( int p0, int p1, int p2 );
    virtual ~Triangle();

    int v0;
    int v1;
    int v2;
    bool toDelete;
};

#endif /* TRIANGLE_H_ */
