/*
 * colormapfunctions.h
 *
 * Created on: Feb 14, 2013
 * @author schurade
 */

#ifndef COLORMAPFUNCTIONS_H_
#define COLORMAPFUNCTIONS_H_

#include "../../algos/colormapbase.h"

class ColormapFunctions
{
public:
    static void addColormap( ColormapBase colormap );
    static void updateColormap( int id, ColormapBase colormap );
    static ColormapBase getColormap( int id );

    static int size();
    static ColormapBase get( int id );

    static void setName( int id, QString name );

    static void deleteColormap( int id );

    static QColor getColor( int colormapID, float value, float min, float max, float lowerThreshold, float upperThreshold, float alpha );

private:
    ColormapFunctions() {}
    virtual ~ColormapFunctions() {}

    static QVector<ColormapBase>m_colormaps;
};

#endif /* COLORMAPFUNCTIONS_H_ */
