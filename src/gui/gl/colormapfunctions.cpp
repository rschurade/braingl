/*
 * colormapfunctions.cpp
 *
 * Created on: Feb 14, 2013
 * @author schurade
 */

#include "colormapfunctions.h"

#include <QDebug>

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

void ColormapFunctions::deleteColormap( int id )
{
    ColormapFunctions::m_colormaps.remove( id );
}

QColor ColormapFunctions::getColor( int colormapID, float value, float min, float max, float lowerThreshold, float upperThreshold, float alpha )
{
    ColormapBase cmap = getColormap( colormapID );
    //qDebug() << value << min << max << lowerThreshold << upperThreshold;
    float value1 = value;
    if ( value < lowerThreshold || value > upperThreshold )
    {
        value1 = 0;
    }
    float value2 = ( value1 - min ) / ( max - min ); //mapped so that s_min = 0, s_max=1

    //qDebug() << value2 << cmap.getColor( qMax( 0.0f, qMin( 1.0f, value2 ) ) );
    if ( value1 > 0 )
    {
        QColor c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, value2 ) ) );
        c.setAlphaF( alpha );
        return c;
    }
    else
    {
        return QColor( 0, 0, 0, 0 );
    }
}
