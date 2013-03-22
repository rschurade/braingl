/*
 * colormapbase.cpp
 *
 * Created on: Feb 12, 2013
 * @author schurade
 */

#include "colormapbase.h"

#include "../gui/gl/glfunctions.h"

#include <QDebug>

ColormapBase::ColormapBase() :
    m_name( "new colormap" )
{
    ColormapPair p0;
    p0.value = 0.0f;
    p0.color = QColor( 0, 0, 0 );

    ColormapPair p1;
    p1.value = 1.0f;
    p1.color = QColor( 255, 255, 255 );

    m_values.push_back( p0 );
    m_values.push_back( p1 );
}


ColormapBase::ColormapBase( QString name, QColor c0, QColor c1 ) :
    m_name( name )
{
    ColormapPair p0;
    p0.value = 0.0f;
    p0.color = c0;

    ColormapPair p1;
    p1.value = 1.0f;
    p1.color = c1;

    m_values.push_back( p0 );
    m_values.push_back( p1 );
}

ColormapBase::ColormapBase( QString name, QVector< ColormapPair > values ) :
    m_name( name ),
    m_values( values )
{

}

ColormapBase::~ColormapBase()
{
}

void ColormapBase::setName( QString name )
{
    m_name = name;
}

QString ColormapBase::getName()
{
    return m_name;
}

void ColormapBase::insertValue( ColormapPair pair )
{
    for ( int i = 1; i < m_values.size(); ++i )
    {
        if ( m_values[i-1].value <= pair.value &&  m_values[i].value > pair.value )
        {
            m_values.insert( i, pair );
            break;
        }
    }
}

void ColormapBase::insertValue( float value, QColor color )
{
    ColormapPair p;
    p.value = value;
    p.color = color;
    insertValue( p );
}

void ColormapBase::removeValue( int index )
{
    if ( index > 0 && index < ( m_values.size() - 1 ) )
    {
        m_values.remove( index, 1 );
    }
}

QColor ColormapBase::getColor( float value )
{
    QColor color1;
    QColor color2;
    float value1 = 0.0;
    float value2 = 1.0;

    for ( int i = 1; i < m_values.size(); ++i )
    {
        if ( value >=m_values[i-1].value &&  value <= m_values[i].value )
        {
            color1 = m_values[i-1].color;
            value1 = m_values[i-1].value;
            color2 = m_values[i].color;
            value2 = m_values[i].value;
        }
    }
    value = ( value - value1 ) / (value2 - value1 );

    QColor out( ( 1.0 - value ) * color1.red()   + value * color2.red(),
                ( 1.0 - value ) * color1.green() + value * color2.green(),
                ( 1.0 - value ) * color1.blue()  + value * color2.blue() );
    return out;
}

QString ColormapBase::getCode()
{
    QString code;
    QColor c0 = m_values[0].color;
    code += "if ( value < 0.0 ) \n    color = vec3( " + QString::number( c0.redF(), 'f', 2 ) + ", " +
                                                   QString::number( c0.greenF(), 'f', 2 ) + ", " +
                                                   QString::number( c0.blueF(), 'f', 2 ) + " ); \n";
    for( int i = 1; i < m_values.size(); ++i )
    {
        QColor c1 = m_values[i-1].color;
        float v1 = m_values[i-1].value;
        QColor c2 = m_values[i].color;
        float v2 = m_values[i].value;
        code += "if ( value >= " + QString::number( v1, 'f', 2 ) + " && value < " + QString::number( v2, 'f', 2 ) + " ) \n";
        code += "{ \n";
        code += "    float _v = ( value - " + QString::number( v1, 'f', 2 ) + " ) / " + QString::number( v2 - v1, 'f', 2 ) + "; \n";
        code += "    color.r = ( 1.0 - _v ) * " + QString::number( c1.redF(), 'f', 2 ) + " + _v * " + QString::number( c2.redF(), 'f', 2 ) + " ; \n";
        code += "    color.g = ( 1.0 - _v ) * " + QString::number( c1.greenF(), 'f', 2 ) + " + _v * " + QString::number( c2.greenF(), 'f', 2 ) + " ;\n ";
        code += "    color.b = ( 1.0 - _v ) * " + QString::number( c1.blueF(), 'f', 2 ) + " + _v * " + QString::number( c2.blueF(), 'f', 2 ) + " ;\n ";
        code += "} \n";
    }

    c0 = m_values.last().color;
    code += "if ( value >= 1.0 ) \n    ";
    code += "{ \n";
    code += "color = vec3( " + QString::number( c0.redF(), 'f', 2 ) + ", " +
                                                   QString::number( c0.greenF(), 'f', 2 ) + ", " +
                                                   QString::number( c0.blueF(), 'f', 2 ) + " ); \n";
    code += "} \n";

    return code;
}

int ColormapBase::size()
{
    return m_values.size();
}

ColormapPair ColormapBase::get( int id )
{
    id = qMax( 0, qMin( m_values.size() - 1, id ) );
    return m_values[id];
}

void ColormapBase::setValue( int id, float value )
{
    m_values[id].value = value;
}

void ColormapBase::setColor( int id, QColor color )
{
    m_values[id].color = color;
}
