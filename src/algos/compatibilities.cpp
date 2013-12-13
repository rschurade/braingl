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

void Compatibilities::init(int nedges){
    qDebug() << "initializing compatibilites with: " << nedges << " #edges";
    this->nedges = nedges;
    idxs = new QVector<QVector<int>* >;
    idxs->resize(nedges);
    comps2 = new QVector<QVector<float>* >;
    comps2->resize(nedges);
    for (int i=0; i<nedges; i++){
        comps2->replace(i,new QVector<float>);
        idxs->replace(i,new QVector<int>);
    }
}

void Compatibilities::addComp(int i, int j, float c){
    idxs->at(i)->append(j);
    comps2->at(i)->append(c);
}
