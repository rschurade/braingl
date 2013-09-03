/*
 * meshalgos.h
 *
 *  Created on: Sep 3, 2013
 *      Author: schurade
 */

#ifndef MESHALGOS_H_
#define MESHALGOS_H_

#include <QList>

class Dataset;

class MeshAlgos
{
public:
    static QList<Dataset*> loopSubdivision( Dataset* ds );

private:
    MeshAlgos() {};
    virtual ~MeshAlgos() {};
};

#endif /* MESHALGOS_H_ */
