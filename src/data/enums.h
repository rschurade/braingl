#ifndef ENUMS_H_
#define ENUMS_H_


enum FN_DATASET_TYPE
{
    FNDT_UNKNOWN,
    FNDT_NIFTI_SCALAR,
    FNDT_NIFTI_VECTOR,
    FNDT_NIFTI_TENSOR,
    FNDT_NIFTI_DWI,
    FNDT_MESH_ASCII,
    FNDT_MESH_BINARY
};

enum FN_ALGO
{
    FNALGO_NONE,
    FNALGO_QBALL,
    FNALGO_TENSORFIT,
    FNALGO_FA,
    FNALGO_EV
};

enum FN_GLOBALS
{
    FNGLOBAL_SAGITTAL = 100,
    FNGLOBAL_CORONAL,
    FNGLOBAL_AXIAL,
    FNGLOBAL_MAX_SAGITTAL,
    FNGLOBAL_MAX_CORONAL,
    FNGLOBAL_MAX_AXIAL,
    FNGLOBAL_SLICE_DX,
    FNGLOBAL_SLICE_DY,
    FNGLOBAL_SLICE_DZ,
    FNGLOBAL_CORONAL_AXIAL,
    FNGLOBAL_SAGITTAL_AXIAL,
    FNGLOBAL_SAGITTAL_CORONAL,
    FNGLOBAL_LAST_PATH,
    FNGLOBAL_SHOW_SAGITTAL,
    FNGLOBAL_SHOW_CORONAL,
    FNGLOBAL_SHOW_AXIAL
};

enum FN_DATASET_PROPERTIES
{
    FNDSP_NAME,
    FNDSP_DIM,
    FNDSP_DATATYPE,
    FNDSP_SIZE,
    FNDSP_NX,
    FNDSP_NY,
    FNDSP_NZ,
    FNDSP_DX,
    FNDSP_DY,
    FNDSP_DZ,
    FNDSP_MIN,
    FNDSP_MAX,
    FNDSP_TYPE,
    FNDSP_CREATED_BY
};

enum FN_DATASET_EDITABLE
{
    FNDSE_LOWER_THRESHOLD = 50,
    FNDSE_UPPER_THRESHOLD,
    FNDSE_COLORMAP,
    FNDSE_INTERPOLATION,
    FNDSE_ALPHA,
    FNDSE_ACTIVE,
    FNDSE_ORDER,
    FNDSE_LOD,
    FNDSE_SCALING,
    FNDSE_RENDER_SLICE,
    FNDSE_RENDER_LOWER_X,
    FNDSE_RENDER_UPPER_X,
    FNDSE_RENDER_LOWER_Y,
    FNDSE_RENDER_UPPER_Y,
    FNDSE_RENDER_LOWER_Z,
    FNDSE_RENDER_UPPER_Z,
    FNDSE_MINMAX_SCALING
};

#endif /* ENUMS_H_ */
