/*
 * scalaralgos.h
 *
 * Created on: 09.07.2012
 * @author Ralph Schurade
 */

#ifndef SCALARALGOS_H_
#define SCALARALGOS_H_

#include <QList>

class Dataset;

class ScalarAlgos
{
public:
    ScalarAlgos();
    virtual ~ScalarAlgos();

    static QList<Dataset*> isoSurface( Dataset* ds, float isoValue = -1 );
    static QList<Dataset*> isoLine( Dataset* ds );
    static QList<Dataset*> distanceMap( Dataset* ds );
    static QList<Dataset*> gauss( Dataset* ds );
    static QList<Dataset*> median( Dataset* ds );
    static QList<Dataset*> createROI( Dataset* ds );

    static QList<Dataset*> createNew( Dataset* ds );

private:
    static double xxgauss(double x, double sigma);
};

#endif /* SCALARALGOS_H_ */
