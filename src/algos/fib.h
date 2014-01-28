/*
 * fib.h
 *
 *  Created on: Jan 16, 2014
 *      Author: Ralph Shurade
 */

#ifndef FIB_H_
#define FIB_H_

#include <QColor>
#include <QVector3D>

class Fib
{
public:
    Fib();
    Fib( const std::vector<QVector3D>& verts );
    Fib( const Fib& other );
    virtual ~Fib();

    unsigned int length() const;

    Fib& operator=( const Fib& other );
    Fib& operator+( const Fib& other );
    QVector3D operator[]( const unsigned int& id ) const;

    void addVert( const QVector3D& vert, const float& data = 0 );
    void addVert( const float& x, const float& y, const float& z, const float& data = 0 );

    void addVerts( std::vector<float>& verts );

    void setVert( const unsigned int id, QVector3D& vert );

    QColor globalColor() const;
    QColor customColor() const;
    void setCustomColor( const QColor& color );

    const std::vector<QVector3D>* getVerts() const;
    QVector3D getVert( const unsigned int& id ) const;
    QVector3D firstVert() const;
    QVector3D lastVert() const;

    const std::vector<QVector3D>* getTangents() const;
    QVector3D getTangent( const unsigned int& id ) const;
    QColor getTangentColor( const unsigned int& id ) const;

    unsigned int getCountDataFields() const;
    const std::vector<float>* getDataField( const unsigned int& fieldId ) const;
    void addDataField();
    void addDataField( std::vector<float>& dataField );
    void setDataField( const unsigned int& fieldId, std::vector<float>& dataField );

    void addData( const unsigned int& fieldId, const float& value );
    float getData( const unsigned int& fieldId, const unsigned int& vertId );
    void  setData( const unsigned int& fieldId, const unsigned int& vertId, const float& value );

    void invert();
    void deleteVert( unsigned int id );
    void deleteLastVert();
    void deletefirstVert();

    void clear();

    float dist( const Fib& other );

private:
    std::vector<QVector3D>m_verts;
    std::vector< std::vector<float> > m_data;

    QColor m_customColor;
    QColor m_globalColor;

    void update();
};

#endif /* FIB_H_ */
