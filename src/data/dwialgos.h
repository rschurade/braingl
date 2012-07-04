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
class TriangleMesh;

class DWIAlgos
{
public:
    static DatasetDWI* qBall( DatasetDWI* ds );

    static Matrix sh_base( Matrix g, int max_order );
private:
    DWIAlgos();
    virtual ~DWIAlgos();

    static Matrix calcQBallBase( Matrix gradients, double lambda, int maxOrder );


    static double sh_base_function( int order, int degree, double theta, double phi );

    static TriangleMesh* writeSHGlyph( DatasetDWI* my_image, unsigned int steps, double color );
};

#endif /* DWIALGOS_H_ */
