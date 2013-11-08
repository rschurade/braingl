/*
 * dwialgos.h
 *
 * Created on: Jun 18, 2012
 * @author Ralph Schurade
 */

#ifndef DWIALGOS_H_
#define DWIALGOS_H_

#include <QList>

class Dataset;

class DWIAlgos
{
public:
    static QList<Dataset*> qBall( Dataset* ds );
    static QList<Dataset*> qBallSharp( Dataset* ds, int order );
    static QList<Dataset*> tensorFit( Dataset* ds );
    static QList<Dataset*> calcFAFromDWI( Dataset* ds );
    static QList<Dataset*> calcEVFromDWI( Dataset* ds );
    static QList<Dataset*> calcFAFromTensor( Dataset* ds );
    static QList<Dataset*> calcEVFromTensor( Dataset* ds );
    static QList<Dataset*> fitBingham( Dataset* ds );
    static QList<Dataset*> bingham2DWI( Dataset* ds );
    static QList<Dataset*> sh2mesh( Dataset* ds );

    static QList<Dataset*> tensorTrack( Dataset* ds );

private:
    DWIAlgos();
    virtual ~DWIAlgos();

    static double pow2( double v )
    {
        return v * v;
    };
    static double pow3( double v )
    {
        return v * v * v;
    };
};

#endif /* DWIALGOS_H_ */
