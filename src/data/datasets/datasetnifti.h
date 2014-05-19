/*
 * datasetnifti.h
 *
 * Created on: May 9, 2012
 * @author Ralph Schurade
 */

#ifndef DATASETNIFTI_H_
#define DATASETNIFTI_H_

#include "dataset.h"

#include "../../thirdparty/nifti/nifti1_io.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QVector>
#include <QMatrix4x4>

class DatasetNifti: public Dataset
{
public:
    DatasetNifti( QDir filename, Fn::DatasetType type, nifti_image* header );
    virtual ~DatasetNifti();

    //nifti_image* getHeader();

    virtual void draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target ) = 0;
    virtual QString getValueAsString( int x, int y, int z ) = 0;

    static QString getNiftiDataType( const int type );

    int getIdFromPos( float x, float y, float z );
    int getIdFromPos( QVector3D pos );
    int getId( int x, int y, int z );
    void getXYZ( int id, int &x, int &y, int &z );
    QList<int>getNeighbourhood3x3( int x, int y, int z );
    QList<int>getNeighbourhoodXxX( int x, int y, int z, int dist );

    QString getSaveFilter();
    QString getDefaultSuffix();

    virtual QColor getColorAtPos( float x, float y, float z );

protected:
    virtual void createTexture() = 0;
    virtual void examineDataset() = 0;

    void parseNiftiHeader();

    nifti_image* m_header;
};

#endif /* DATASETNIFTI_H_ */
