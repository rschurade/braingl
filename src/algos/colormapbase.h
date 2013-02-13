/*
 * colormapbase.h
 *
 *  Created on: Feb 12, 2013
 *      Author: schurade
 */

#ifndef COLORMAPBASE_H_
#define COLORMAPBASE_H_

#include <QtGui/QColor>
#include <QtCore/QVector>

struct ColormapPair
{
    float value;
    QColor color;
};

class ColormapBase
{
public:
    ColormapBase();
    ColormapBase( QColor c0, QColor c1 );
    virtual ~ColormapBase();

    void insertValue( ColormapPair value );
    void insertValue( float value, QColor );
    void removeValue( int index );

    void setValue( int id, float value );
    void setColor( int id, QColor color );

    int size();
    ColormapPair get( int id );

    QColor getColor( float value );
    QString getCode();

private:
    QVector< ColormapPair >m_values;

};

#endif /* COLORMAPBASE_H_ */
