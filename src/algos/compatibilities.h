/*
 * compatibilities.h
 *
 *  Created on: Dec 5, 2013
 *      Author: boettgerj
 */

#ifndef COMPATIBILITIES_H_
#define COMPATIBILITIES_H_

#include <QVector>

class Compatibilities
{
public:
    Compatibilities(int nedges);
    virtual ~Compatibilities();

    void addComp(int i, int j, float c);
    QVector< QVector<int>* >* idxs;
    QVector< std::vector<float>* >* comps2;

private:
    int nedges;
    void init(int nedges);
};

#endif /* COMPATIBILITIES_H_ */
