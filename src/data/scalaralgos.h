/*
 * scalaralgos.h
 *
 *  Created on: 09.07.2012
 *      Author: Ralph
 */

#ifndef SCALARALGOS_H_
#define SCALARALGOS_H_

#include <QtCore/QList>

class DatasetScalar;

class ScalarAlgos
{
public:
    ScalarAlgos();
    virtual ~ScalarAlgos();

    static QList<Dataset*> isoSurface( DatasetScalar* ds );
};

#endif /* SCALARALGOS_H_ */
