/*
 * dwialgos.h
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */

#ifndef DWIALGOS_H_
#define DWIALGOS_H_

class DatasetDWI;
class DatasetNifti;
class TriangleMesh;

class DWIAlgos
{
public:
    static DatasetDWI* qBall( DatasetDWI* ds );
    static DatasetDWI* tensorFit( DatasetDWI* ds );


private:
    DWIAlgos();
    virtual ~DWIAlgos();


};

#endif /* DWIALGOS_H_ */
