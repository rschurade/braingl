/*
 * connection.h
 *
 *  Created on: Apr 30, 2013
 *      Author: boettgerj
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <QVector3D>

class Connection
{
public:
    Connection( QVector3D fn, QVector3D tn, float v );
    virtual ~Connection();

    QVector3D fn, diff;
    float v;
    float r,g,b;
};

#endif /* CONNECTION_H_ */
