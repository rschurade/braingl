/*
 * datasetnifti.h
 *
 *  Created on: May 9, 2012
 *      Author: schurade
 */

#ifndef DATASETNIFTI_H_
#define DATASETNIFTI_H_

#include "../glew/include/glew.h"

#include "nifti/nifti1_io.h"

#include "dataset.h"

class DatasetNifti : public Dataset
{
public:
    DatasetNifti(  QString filename, FN_DATASET_TYPE type, void* data  );
    virtual ~DatasetNifti();

    void parseNiftiHeader( nifti_image* header );

    GLuint getTextureGLuint();

    const char* getAuxFile() const
    {
        return aux_file;
    }

    int getByteorder() const
    {
        return byteorder;
    }

    float getCalMax() const
    {
        return cal_max;
    }

    float getCalMin() const
    {
        return cal_min;
    }

    int getDatatype() const
    {
        return datatype;
    }

    const char* getDescrip() const
    {
        return descrip;
    }

    const int* getDim() const
    {
        return dim;
    }

    float getDt() const
    {
        return dt;
    }

    float getDu() const
    {
        return du;
    }

    float getDv() const
    {
        return dv;
    }

    float getDw() const
    {
        return dw;
    }

    float getDx() const
    {
        return dx;
    }

    float getDy() const
    {
        return dy;
    }

    float getDz() const
    {
        return dz;
    }

    char* getFname() const
    {
        return fname;
    }

    int getFreqDim() const
    {
        return freq_dim;
    }

    char* getIname() const
    {
        return iname;
    }

    int getInameOffset() const
    {
        return iname_offset;
    }

    int getIntentCode() const
    {
        return intent_code;
    }

    const char* getIntentName() const
    {
        return intent_name;
    }

    float getIntentP1() const
    {
        return intent_p1;
    }

    float getIntentP2() const
    {
        return intent_p2;
    }

    float getIntentP3() const
    {
        return intent_p3;
    }

    void* getData() const
    {
        return m_data;
    }

    int getNbyper() const
    {
        return nbyper;
    }

    int getNdim() const
    {
        return ndim;
    }

    int getNiftiType() const
    {
        return nifti_type;
    }

    int getNt() const
    {
        return nt;
    }

    int getNu() const
    {
        return nu;
    }

    int getNv() const
    {
        return nv;
    }

    size_t getNvox() const
    {
        return nvox;
    }

    int getNw() const
    {
        return nw;
    }

    int getNx() const
    {
        return nx;
    }

    int getNy() const
    {
        return ny;
    }

    int getNz() const
    {
        return nz;
    }

    int getPhaseDim() const
    {
        return phase_dim;
    }

    const float* getPixdim() const
    {
        return pixdim;
    }

    int getQformCode() const
    {
        return qform_code;
    }

    mat44 getQtoIjk() const
    {
        return qto_ijk;
    }

    mat44 getQtoXyz() const
    {
        return qto_xyz;
    }

    float getQuaternB() const
    {
        return quatern_b;
    }

    float getSclInter() const
    {
        return scl_inter;
    }

    float getSclSlope() const
    {
        return scl_slope;
    }

    int getSformCode() const
    {
        return sform_code;
    }

    int getSliceCode() const
    {
        return slice_code;
    }

    int getSliceDim() const
    {
        return slice_dim;
    }

    float getSliceDuration() const
    {
        return slice_duration;
    }

    int getSliceEnd() const
    {
        return slice_end;
    }

    int getSliceStart() const
    {
        return slice_start;
    }

    mat44 getStoIjk() const
    {
        return sto_ijk;
    }

    mat44 getStoXyz() const
    {
        return sto_xyz;
    }

    int getSwapsize() const
    {
        return swapsize;
    }

    int getTimeUnits() const
    {
        return time_units;
    }

    float getToffset() const
    {
        return toffset;
    }

    int getXyzUnits() const
    {
        return xyz_units;
    }

protected:
    virtual void createTexture() = 0;

    void* m_data;

    GLuint m_textureGLuint;

    int ndim;       /*!< last dimension greater than 1 (1..7) */
    int nx;         /*!< dimensions of grid array             */
    int ny;         /*!< dimensions of grid array             */
    int nz;         /*!< dimensions of grid array             */
    int nt;         /*!< dimensions of grid array             */
    int nu;         /*!< dimensions of grid array             */
    int nv;         /*!< dimensions of grid array             */
    int nw;         /*!< dimensions of grid array             */
    int dim[8];     /*!< dim[0]=ndim, dim[1]=nx, etc.         */
    size_t nvox;    /*!< number of voxels = nx*ny*nz*...*nw   */
    int nbyper;     /*!< bytes per voxel, matches datatype    */
    int datatype;   /*!< type of data in voxels: DT_* code    */

    float dx;           /*!< grid spacings      */
    float dy;           /*!< grid spacings      */
    float dz;           /*!< grid spacings      */
    float dt;           /*!< grid spacings      */
    float du;           /*!< grid spacings      */
    float dv;           /*!< grid spacings      */
    float dw;           /*!< grid spacings      */
    float pixdim[8];    /*!< pixdim[1]=dx, etc. */

    float scl_slope;    /*!< scaling parameter - slope        */
    float scl_inter;    /*!< scaling parameter - intercept    */

    float cal_min;      /*!< calibration parameter, minimum   */
    float cal_max;      /*!< calibration parameter, maximum   */

    int qform_code;     /*!< codes for (x,y,z) space meaning  */
    int sform_code;     /*!< codes for (x,y,z) space meaning  */

    int freq_dim;       /*!< indexes (1,2,3, or 0) for MRI    */
    int phase_dim;      /*!< directions in dim[]/pixdim[]     */
    int slice_dim;      /*!< directions in dim[]/pixdim[]     */

    int slice_code;         /*!< code for slice timing pattern    */
    int slice_start;        /*!< index for start of slices        */
    int slice_end;          /*!< index for end of slices          */
    float slice_duration;   /*!< time between individual slices   */

    /*! quaternion transform parameters
     [when writing a dataset, these are used for qform, NOT qto_xyz]   */
    float quatern_b, quatern_c, quatern_d, qoffset_x, qoffset_y, qoffset_z, qfac;

    mat44 qto_xyz;      /*!< qform: transform (i,j,k) to (x,y,z) */
    mat44 qto_ijk;      /*!< qform: transform (x,y,z) to (i,j,k) */

    mat44 sto_xyz;      /*!< sform: transform (i,j,k) to (x,y,z) */
    mat44 sto_ijk;      /*!< sform: transform (x,y,z) to (i,j,k) */

    float toffset;      /*!< time coordinate offset */

    int xyz_units;      /*!< dx,dy,dz units: NIFTI_UNITS_* code  */
    int time_units;     /*!< dt       units: NIFTI_UNITS_* code  */

    int nifti_type;     /*!< 0==ANALYZE,
                             1==NIFTI-1 (1 file),
                             2==NIFTI-1 (2 files),
                             3==NIFTI-ASCII (1 file) */
    int intent_code;        /*!< statistic type (or something)       */
    float intent_p1;        /*!< intent parameters                   */
    float intent_p2;        /*!< intent parameters                   */
    float intent_p3;        /*!< intent parameters                   */
    char intent_name[16];   /*!< optional description of intent data */

    char descrip[80];       /*!< optional text to describe dataset   */
    char aux_file[24];      /*!< auxiliary filename                  */

    char *fname;            /*!< header filename (.hdr or .nii)         */
    char *iname;            /*!< image filename  (.img or .nii)         */
    int iname_offset;       /*!< offset into iname where data starts    */
    int swapsize;           /*!< swap unit in image data (might be 0)   */
    int byteorder;          /*!< byte order on disk (MSB_ or LSB_FIRST) */
};

#endif /* DATASETNIFTI_H_ */
