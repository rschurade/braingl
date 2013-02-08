/*
 * fiberalgos.h
 *
 *  Created on: 09.07.2012
 *      Author: Ralph Schurade
 */

#ifndef FIBERALGOS_H_
#define FIBERALGOS_H_

#include <QtCore/QList>

class Dataset;

class FiberAlgos
{
public:
    FiberAlgos();
    virtual ~FiberAlgos();

    static QList<Dataset*> thinOut( Dataset* ds );
    static QList<Dataset*> tractDensity( Dataset* ds );
};

#endif /* FIBERALGOS_H_ */
