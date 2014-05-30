/*
 * connection.cpp
 *
 *  Created on: Apr 30, 2013
 *      Author: boettgerj
 */

#include "connection.h"

#include "qmath.h"

Connection::Connection( QVector3D fn, QVector3D diff, float v ) :
    fn( fn ),
    diff( diff ),
    v( v )
{
       QVector3D diffn = diff.normalized();

       r=qAbs(diffn.x());
       g=qAbs(diffn.y());
       b=qAbs(diffn.z());
}

Connection::~Connection()
{

}

