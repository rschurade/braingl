/*
 * point.cpp
 *
 *  Created on: Sep 4, 2013
 *      Author: schurade
 */

#include "point.h"

Point::Point() :
    position( 0, 0, 0 ),
    newID( -1 ),
    toDelete( false )
{
}

Point::Point( QVector3D pos ) :
    position( pos ),
    newID( -1 ),
    toDelete( false )
{
}

Point::~Point()
{
}

