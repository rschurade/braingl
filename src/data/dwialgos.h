/*
 * dwialgos.h
 *
 *  Created on: Jun 18, 2012
 *      Author: schurade
 */

#ifndef DWIALGOS_H_
#define DWIALGOS_H_

/*** newmat include ***/
//#define WANT_MATH
//#define WANT_STREAM
#define use_namespace
#include <newmat/newmat.h>
//#include <newmat/newmatap.h>
//#include <newmat/newmatio.h>
//#include <newmat/newmatrm.h>
//#include <newmat/precisio.h>
using namespace NEWMAT;

class DatasetDWI;
class DatasetNifti;

class DWIAlgos
{
public:
    static DatasetNifti* qBall( DatasetDWI* ds );

private:
    DWIAlgos();
    virtual ~DWIAlgos();

    static Matrix calcQBallBase( Matrix gradients, double lambda, int maxOrder );
    static Matrix sh_base( Matrix g, int max_order );

    static double sh_base_function( int order, int degree, double theta, double phi );

    //template<typename T> void multQBall( T* data, DatasetDWI* ds, boost::shared_ptr<Matrix> qBallBase );
};

#endif /* DWIALGOS_H_ */
