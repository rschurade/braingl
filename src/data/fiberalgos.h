/*
 * fiberalgos.h
 *
 *  Created on: 09.07.2012
 *      Author: Ralph
 */

#ifndef FIBERALGOS_H_
#define FIBERALGOS_H_

#include <QtCore/QList>

class DatasetFibers;

class FiberAlgos
{
public:
    FiberAlgos();
    virtual ~FiberAlgos();

    static QList<Dataset*> thinOut( DatasetFibers* ds );
};

#endif /* FIBERALGOS_H_ */
