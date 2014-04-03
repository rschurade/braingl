/*
 * meshalgos.h
 *
 *  Created on: Sep 3, 2013
 *      Author: schurade
 */

#ifndef MESHALGOS_H_
#define MESHALGOS_H_

#include <vector>
#include <QList>

class Dataset;
class TriangleMesh2;

class MeshAlgos
{
public:
    static QList<Dataset*> loopSubdivision( Dataset* ds );
    static QList<Dataset*> meshTimeSeries( Dataset* ds, TriangleMesh2* mesh );
    static QList<Dataset*> biggestComponent( Dataset* ds );
    static TriangleMesh2* pruneMesh( TriangleMesh2* mesh, std::vector<int>component );
    static QList<Dataset*> decimate( Dataset* ds );

private:
    MeshAlgos() {};
    virtual ~MeshAlgos() {};
};

#endif /* MESHALGOS_H_ */
