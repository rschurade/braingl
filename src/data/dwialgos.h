/*
 * dwialgos.h
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */

#ifndef DWIALGOS_H_
#define DWIALGOS_H_

#include <newmat/newmat.h>

class DatasetDWI;
class DatasetNifti;

class DWIAlgos
{
public:
    static DatasetDWI* qBall( DatasetDWI* ds );

private:
    DWIAlgos();
    virtual ~DWIAlgos();

    static NEWMAT::Matrix calcQBallBase( NEWMAT::Matrix gradients, double lambda, int maxOrder );
    static NEWMAT::Matrix sh_base( NEWMAT::Matrix g, int max_order );

    static double sh_base_function( int order, int degree, double theta, double phi );

    //template<typename T> void multQBall( T* data, DatasetDWI* ds, boost::shared_ptr<Matrix> qBallBase );
};

#endif /* DWIALGOS_H_ */
