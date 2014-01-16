/*
 * colormapbase.h
 *
 * Created on: Feb 12, 2013
 * @author schurade
 */

#ifndef COLORMAPBASE_H_
#define COLORMAPBASE_H_

#include <QColor>
#include <QVector>
#include <QString>

struct ColormapPair
{
    float value;
    QColor color;
};

class ColormapBase
{
public:
    ColormapBase();
    ColormapBase( QString name, QColor c0, QColor c1 );
    ColormapBase( QString name, QVector< ColormapPair >values );
    ColormapBase( QList<QVariant> cm );
    virtual ~ColormapBase();

    void insertValue( ColormapPair value );
    void insertValue( float value, QColor );
    void removeValue( int index );

    void setValue( int id, float value );
    void setColor( int id, QColor color );

    void setName( QString name );
    QString getName();

    int size();
    ColormapPair get( int id );

    QColor getColor( float value );
    QString getCode();

    QList<QVariant>serialize();

private:
    QString m_name;
    QVector< ColormapPair >m_values;

};

#endif /* COLORMAPBASE_H_ */
