/*
 * fiberalgos.h
 *
 * Created on: 09.07.2012
 * @author Ralph Schurade
 */

#ifndef FIBERALGOS_H_
#define FIBERALGOS_H_

#include <QList>

class Dataset;

class FiberAlgos
{
public:
    FiberAlgos();
    virtual ~FiberAlgos();

    static QList<Dataset*> thinOut( Dataset* ds );
    static QList<Dataset*> tractDensity( Dataset* ds );
    static QList<Dataset*> tractColor( Dataset* ds );
    static QList<Dataset*> cutSelecteded( Dataset* ds );
    static QList<Dataset*> downSample( Dataset* ds );
    static QList<Dataset*> unfold( Dataset* ds );
};

#endif /* FIBERALGOS_H_ */
