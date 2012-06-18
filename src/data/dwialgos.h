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

class DWIAlgos
{
public:
    static DatasetNifti* qBall( DatasetDWI* );

private:
    DWIAlgos();
    virtual ~DWIAlgos();
};

#endif /* DWIALGOS_H_ */
