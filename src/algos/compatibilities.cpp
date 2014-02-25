/*
 * compatibilities.cpp
 *
 *  Created on: Dec 5, 2013
 *      Author: boettgerj
 */

#include "compatibilities.h"

#include <QDebug>

Compatibilities::Compatibilities(int nedges)
{
    init(nedges);
}

Compatibilities::~Compatibilities()
{
    // TODO Auto-generated destructor stub
}

void Compatibilities::init( int nedges )
{
    qDebug() << "initializing compatibilites with: " << nedges << " #edges";
    this->nedges = nedges;
    idxs = new std::vector<std::vector<int>*>;
    idxs->resize( nedges );
    comps2 = new std::vector<std::vector<float>*>;
    comps2->resize( nedges );
    for ( int i = 0; i < nedges; i++ )
    {
        comps2->at( i ) = new std::vector<float>;
        idxs->at( i ) = new std::vector<int>;
    }
}

void Compatibilities::addComp( int i, int j, float c )
{
    idxs->at( i )->push_back( j );
    comps2->at( i )->push_back( c );
}
