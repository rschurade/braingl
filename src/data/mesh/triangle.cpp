/*
 * triangle.cpp
 *
 *  Created on: Sep 4, 2013
 *      Author: schurade
 */

#include "triangle.h"

Triangle::Triangle() :
    v0( -1 ),
    v1( -1 ),
    v2( -1 ),
    toDelete( false )
{
}

Triangle::Triangle( int p0, int p1, int p2 ) :
    v0( p0 ),
    v1( p1 ),
    v2( p2 ),
    toDelete( false )
{
}

Triangle::~Triangle()
{
}

