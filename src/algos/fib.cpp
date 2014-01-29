/*
 * fib.cpp
 *
 *  Created on: Jan 16, 2014
 *      Author: Ralph Schurade
 */

#include "fib.h"

#include <QDebug>

Fib::Fib()
{
    std::vector<float>data;
    m_data.push_back( data );
}

Fib::Fib( const std::vector<QVector3D>& verts )
{
    m_verts = verts;
    std::vector<float>data( verts.size(), 0 );
    m_data.push_back( data );
}

Fib::Fib( const Fib& other )
{
    m_verts = *( other.getVerts() );
    for ( unsigned int i = 0; i < other.getCountDataFields(); ++i )
    {
        m_data.push_back( *( other.getDataField( i ) ) );
    }
    m_customColor = other.customColor();
    m_globalColor = other.globalColor();
}

Fib::~Fib()
{
    m_verts.clear();
    m_data.clear();
}

unsigned int Fib::length() const
{
    return m_verts.size();
}

Fib& Fib::operator=( const Fib& other )
{
    m_verts = *( other.getVerts() );
    m_data.clear();
    for ( unsigned int i = 0; i < other.getCountDataFields(); ++i )
    {
        m_data.push_back( *( other.getDataField( i ) ) );
    }
    m_customColor = other.customColor();
    m_globalColor = other.globalColor();

    return *this;
}

Fib& Fib::operator+( const Fib& other )
{
    m_verts.insert( m_verts.end(), other.getVerts()->begin(), other.getVerts()->end() );
    if( m_data.size() == other.getCountDataFields() )
    {
        for ( unsigned int i = 0; i < m_data.size(); ++i )
        {
            m_data[i].insert( m_data[i].end(), other.getDataField( i )->begin(), other.getDataField( i)->end() );
        }
    }
    return *this;
}

QVector3D Fib::operator[]( const unsigned int& id ) const
{
    return m_verts[id];
}

void Fib::addVert( const QVector3D& vert, const float& value )
{
    m_verts.push_back( vert );
    for ( unsigned int i = 0; i < m_data.size(); ++i )
    {
        m_data[i].push_back( value );
    }
}

void Fib::addVert( const float& x, const float& y, const float& z, const float& value )
{
    addVert( QVector3D( x, y, z ), value );
}

void Fib::addVerts( std::vector<float>& verts )
{
    for ( unsigned int i = 0; i < verts.size() / 3; ++i )
    {
        QVector3D vec( verts[i*3], verts[i*3+1], verts[i*3+2] );
        m_verts.push_back( vec );
    }
}

void Fib::setVert( const unsigned int id, QVector3D& vert )
{
    if ( id < m_verts.size() )
    {
        m_verts[id] = vert;
    }
    else
    {
        qDebug() << "Fib: tried to access vert id out of range" << __LINE__;
        exit( 0 );
    }
}

QColor Fib::globalColor() const
{
    QVector3D c( fabs( m_verts.front().x() - m_verts.back().x() ), fabs( m_verts.front().y() - m_verts.back().y() ), fabs( m_verts.front().z() - m_verts.back().z() ) );
    c.normalize();
    return QColor( c.x() * 255, c.y() * 255, c.z() * 255 );
    //return m_globalColor;
}

QColor Fib::customColor() const
{
    return m_customColor;
}

void Fib::setCustomColor( const QColor& color )
{
    m_customColor = color;
}

const std::vector<QVector3D>* Fib::getVerts() const
{
    return &m_verts;
}

QVector3D Fib::getVert( const unsigned int& id ) const
{
    if ( id < m_verts.size() )
    {
        return m_verts[id];
    }
    else
    {
        qDebug() << "Fib: tried to access vert id out of range" << __LINE__;
        exit( 0 );
    }
}

QVector3D Fib::firstVert() const
{
    if ( !m_verts.empty() )
    {
        return m_verts.front();
    }
    else
    {
        qDebug() << "Fib: tried to access first vert of empty fib" << __LINE__;
        exit( 0 );
    }
}

QVector3D Fib::lastVert() const
{
    if ( !m_verts.empty() )
    {
        return m_verts.back();
    }
    else
    {
        qDebug() << "Fib: tried to access last vert of empty fib" << __LINE__;
        exit( 0 );
    }
}

const std::vector<QVector3D>* Fib::getTangents() const
{
    qDebug() << "to implement Fib::getTangents()" << __LINE__;
    exit( 0 );
}

QVector3D Fib::getTangent( const unsigned int& id ) const
{
    if ( id < m_verts.size() )
    {
        unsigned int p1 = qMin( 0u, qMax( 0u, id - 1 ) );
        unsigned int p2 = qMax( id + 1, (unsigned int)m_verts.size() - 1 );
        QVector3D t = ( m_verts[p1] -m_verts[p2] ).normalized();
        return t;
    }
    else
    {
        qDebug() << "Fib: tried to access tangent id out of range" << __LINE__;
        exit( 0 );
    }
}

QColor Fib::getTangentColor( const unsigned int& id ) const
{
    QVector3D t = getTangent( id );
    return QColor( fabs( t.x() ) * 255, fabs( t.y() ) * 255, fabs( t.z() ) * 255 );
}

unsigned int Fib::getCountDataFields() const
{
    return m_data.size();
}

const std::vector<float>* Fib::getDataField( const unsigned int& fieldId ) const
{
    if ( fieldId < m_data.size() )
    {
        return &( m_data[fieldId] );
    }
    else
    {
        qDebug() << "Fib: tried to access data field out of range" << __LINE__;
        exit( 0 );
    }
}

void Fib::addDataField()
{
    std::vector<float> dataField( m_verts.size() );
    m_data.push_back( dataField );
}

void Fib::addDataField( std::vector<float>& dataField )
{
    m_data.push_back( dataField );
}

void Fib::setDataField( const unsigned int& fieldId, std::vector<float>& dataField )
{
    if ( fieldId < m_data.size() )
    {
        m_data[fieldId] = dataField;
    }
    else
    {
        qDebug() << "Fib: tried to access data field out of range" << __LINE__;
        exit( 0 );
    }
}

float Fib::getData( const unsigned int& fieldId, const unsigned int& vertId )
{
    if ( fieldId < m_data.size() )
    {
        if( vertId < m_data[fieldId].size() )
        {
            return m_data[fieldId][vertId];
        }
        else
        {
            qDebug() << "Fib: tried to access data point out of range" << __LINE__;
            exit( 0 );
        }
    }
    else
    {
        qDebug() << "Fib: tried to access data field " << fieldId << " out of range" << __LINE__;
        exit( 0 );
    }
}

void Fib::addData( const unsigned int& fieldId, const float& value )
{
    if ( fieldId < m_data.size() )
    {
        m_data[fieldId].push_back( value );
    }
    else
    {
        qDebug() << "Fib: tried to access data field out of range" << __LINE__;
        exit( 0 );
    }
}

void Fib::setData( const unsigned int& fieldId, const unsigned int& vertId, const float& value )
{
    if ( fieldId < m_data.size() )
    {
        if( vertId < m_data[fieldId].size() )
        {
            m_data[fieldId][vertId] = value;
        }
        else
        {
            qDebug() << "Fib: tried to access data point out of range" << __LINE__;
            exit( 0 );
        }
    }
    else
    {
        qDebug() << "Fib: tried to access data field out of range" << __LINE__;
        exit( 0 );
    }
}

void Fib::update()
{
    if ( m_verts.size() > 1 )
    {
        QVector3D diff = m_verts.front() - m_verts.back();
        m_globalColor.setRedF( fabs( diff.x() ) );
        //, fabs( diff.y() ), fabs( diff.z() ) );
    }
}

void Fib::invert()
{
    if ( m_verts.size() < 2 )
    {
        return;
    }
    // inverts verts
    std::reverse( m_verts.begin(), m_verts.end() );

    for ( unsigned int i = 0; i < m_data.size(); ++i )
    {
        std::reverse( m_data[i].begin(), m_data[i].end() );
    }
}

void Fib::deleteVert( unsigned int id )
{
    if ( m_verts.size() > id + 1 )
    {
        m_verts.erase( m_verts.begin() + id );
        for ( unsigned int i = 0; i < m_data.size(); ++i )
        {
            m_data[i].erase( m_data[i].begin() + id );
        }
    }
}

void Fib::deleteLastVert()
{
    if ( m_verts.size() > 0 )
    {
        m_verts.pop_back();
        for ( unsigned int i = 0; i < m_data.size(); ++i )
        {
            m_data[i].pop_back();
        }
    }
}


void Fib::deletefirstVert()
{
    if ( m_verts.size() > 0 )
    {
        m_verts.erase( m_verts.begin() );
        for ( unsigned int i = 0; i < m_data.size(); ++i )
        {
            m_data[i].erase( m_data[i].begin() );
        }
    }
}

float Fib::dist( const Fib& other )
{
    float distSum = 0;
    float minSum;
    for ( unsigned int i = 0; i < length(); ++i )
    {
        minSum = std::numeric_limits<float>::max();

        for( unsigned int k = 0; k < other.length(); ++k )
        {
            minSum = qMin( minSum, (float)( m_verts[i] - other[k] ).length() );
        }
        distSum += minSum;
    }
    distSum /= length();

    return distSum;
}

void Fib::clear()
{
    m_verts.clear();
    m_data.clear();
}
