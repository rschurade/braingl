#ifndef ENUMS_H_
#define ENUMS_H_

#include <QtCore/QMetaType>

namespace Fn
{
    enum class DatasetType : int
    {
        UNKNOWN,
        NIFTI_SCALAR,
        NIFTI_VECTOR,
        NIFTI_TENSOR,
        NIFTI_SH,
        NIFTI_DWI,
        NIFTI_BINGHAM,
        NIFTI_ANY,
        MESH_ASCII,
        MESH_BINARY,
        MESH_ISOSURFACE,
        FIBERS
    };

    enum class Colormap : int
    {
        GRAY,
        RAINBOW1,
        RAINBOW2,
        BLUEWHITERED,
        NONE // used for RGB datasets, takes values directly from texture
             // other colormaps use float packing
    };

    enum class Algo : int
    {
        NONE,
        TEST,
        QBALL,
        QBALL4,
        QBALL6,
        QBALL8,
        TENSORFIT,
        FA,
        EV,
        BINGHAM,
        ISOSURFACE,
        TENSOR_TRACK,
        CROSSING_TRACK,
        BINGHAM_2_TENSOR,
        FIBER_THINNING,
        TRACT_DENSITY
    };

    enum class Orient : int
    {
        NONE,
        AXIAL,
        CORONAL,
        SAGITTAL
    };

    enum class Property : int
    {
        // dataset properties
        NAME,
        DIM,
        DATATYPE,
        SIZE,
        NX,
        NY,
        NZ,
        DX,
        DY,
        DZ,
        MIN,
        MAX,
        TYPE,
        CREATED_BY,
        FILENAME,
        LOWER_THRESHOLD,
        UPPER_THRESHOLD,
        COLORMAP,
        INTERPOLATION,
        ALPHA,
        ACTIVE,
        ORDER,
        LOD,
        SCALING,
        FA_THRESHOLD,
        EV_THRESHOLD,
        GAMMA,
        OFFSET,
        RENDER_SLICE,
        RENDER_LOWER_X,
        RENDER_UPPER_X,
        RENDER_LOWER_Y,
        RENDER_UPPER_Y,
        RENDER_LOWER_Z,
        RENDER_UPPER_Z,
        MINMAX_SCALING,
        BVALUE,
        TENSOR_RENDERMODE,
        ISO_VALUE,
        NUM_POINTS,
        NUM_LINES,
        FIBER_COLORMODE,
        FIBER_RENDERMODE,
        TEXTURE_GLUINT,
        DATASET_POINTER,
        NEW_DATASET,
        DATASET_LIST,
        // Settings
        SETTING_LOCK_WIDGETS,
        SETTING_RENDER_CROSSHAIRS,
        // Global Settings for rendering
        GLOBALS,
        GLOBAL_SAGITTAL,
        GLOBAL_CORONAL,
        GLOBAL_AXIAL,
        GLOBAL_MAX_SAGITTAL,
        GLOBAL_MAX_CORONAL,
        GLOBAL_MAX_AXIAL,
        GLOBAL_SLICE_DX,
        GLOBAL_SLICE_DY,
        GLOBAL_SLICE_DZ,
        GLOBAL_CORONAL_AXIAL,
        GLOBAL_SAGITTAL_AXIAL,
        GLOBAL_SAGITTAL_CORONAL,
        GLOBAL_LAST_PATH,
        GLOBAL_SHOW_SAGITTAL,
        GLOBAL_SHOW_CORONAL,
        GLOBAL_SHOW_AXIAL,
    // settings that need a datastore signal to update widgets need to be inserted above this line
        GLOBAL_ZOOM,
        GLOBAL_MOVEX,
        GLOBAL_MOVEY,
        GLOBAL_BBX,
        GLOBAL_BBY,
        GLOBAL_VIEW
    };
}

Q_DECLARE_METATYPE( Fn::DatasetType )
Q_DECLARE_METATYPE( Fn::Colormap )
Q_DECLARE_METATYPE( Fn::Algo )
Q_DECLARE_METATYPE( Fn::Orient )
Q_DECLARE_METATYPE( Fn::Property )


#endif /* ENUMS_H_ */
