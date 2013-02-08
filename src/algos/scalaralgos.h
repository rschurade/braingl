/*
 * scalaralgos.h
 *
 *  Created on: 09.07.2012
 *      Author: Ralph Schurade
 */

#ifndef SCALARALGOS_H_
#define SCALARALGOS_H_

#include <QtCore/QList>

class Dataset;

class ScalarAlgos
{
public:
    ScalarAlgos();
    virtual ~ScalarAlgos();

    static QList<Dataset*> isoSurface( Dataset* ds );
};

#endif /* SCALARALGOS_H_ */
