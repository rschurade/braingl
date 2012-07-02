/*
 * dwialgos.h
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */

#ifndef DWIALGOS_H_
#define DWIALGOS_H_

#include "../thirdparty/newmat10/newmat.h"

class DatasetDWI;
class DatasetNifti;

class DWIAlgos
{
public:
    static DatasetDWI* qBall( DatasetDWI* ds );

private:
    DWIAlgos();
    virtual ~DWIAlgos();

    static Matrix calcQBallBase( Matrix gradients, double lambda, int maxOrder );
    static Matrix sh_base( Matrix g, int max_order );

    static double sh_base_function( int order, int degree, double theta, double phi );

    //template<typename T> void multQBall( T* data, DatasetDWI* ds, boost::shared_ptr<Matrix> qBallBase );
};

#endif /* DWIALGOS_H_ */
