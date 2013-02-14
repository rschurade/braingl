/*
 * colormapfunctions.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: schurade
 */

#include "colormapfunctions.h"

QVector<ColormapBase>ColormapFunctions::m_colormaps;

void ColormapFunctions::addColormap( ColormapBase colormap )
{
    ColormapFunctions::m_colormaps.push_back( colormap );
}

void ColormapFunctions::updateColormap( int id, ColormapBase colormap )
{
    ColormapFunctions::m_colormaps.replace( id, colormap );
}

ColormapBase ColormapFunctions::getColormap( int id )
{
    return ColormapFunctions::m_colormaps[ qMax( 0, qMin( id, ColormapFunctions::m_colormaps.size() - 1 ) ) ];
}

int ColormapFunctions::size()
{
    return ColormapFunctions::m_colormaps.size();
}

ColormapBase ColormapFunctions::get( int id )
{
    return ColormapFunctions::m_colormaps[id];
}

void ColormapFunctions::setName( int id, QString name )
{
    ColormapFunctions::m_colormaps[id].setName( name );
}
