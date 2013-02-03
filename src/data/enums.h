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

    };

    enum class Global : int
    {
        // Settings
        LOCK_WIDGETS,
        RENDER_CROSSHAIRS,
        // Global Settings for rendering
        SAGITTAL,
        CORONAL,
        AXIAL,
        MAX_SAGITTAL,
        MAX_CORONAL,
        MAX_AXIAL,
        SLICE_DX,
        SLICE_DY,
        SLICE_DZ,
        CORONAL_AXIAL,
        SAGITTAL_AXIAL,
        SAGITTAL_CORONAL,
        LAST_PATH,
        SHOW_SAGITTAL,
        SHOW_CORONAL,
        SHOW_AXIAL,
    // settings that need a datastore signal to update widgets need to be inserted above this line
        ZOOM,
        MOVEX,
        MOVEY,
        BBX,
        BBY,
        VIEW
    };

    struct Prop2String {
        static QString s( Property p )
        {
            switch( p )
            {
            case Property::NAME: return QString( "name" ); break;
            case Property::DIM: return QString( "dim" ); break;
            case Property::DATATYPE: return QString( "data type" ); break;
            case Property::SIZE: return QString( "size" ); break;
            case Property::NX: return QString( "nx" ); break;
            case Property::NY: return QString( "ny" ); break;
            case Property::NZ: return QString( "nz" ); break;
            case Property::DX: return QString( "dx" ); break;
            case Property::DY: return QString( "dy" ); break;
            case Property::DZ: return QString( "dz" ); break;
            case Property::MIN: return QString( "min" ); break;
            case Property::MAX: return QString( "max" ); break;
            case Property::TYPE: return QString( "type" ); break;
            case Property::CREATED_BY: return QString( "created by" ); break;
            case Property::FILENAME: return QString( "filename" ); break;
            case Property::LOWER_THRESHOLD: return QString( "lower threshold" ); break;
            case Property::UPPER_THRESHOLD: return QString( "upper threshold" ); break;
            case Property::COLORMAP: return QString( "colormap" ); break;
            case Property::INTERPOLATION: return QString( "interpolation" ); break;
            case Property::ALPHA: return QString( "alpha" ); break;
            case Property::ACTIVE: return QString( "active" ); break;
            case Property::ORDER: return QString( "order" ); break;
            case Property::LOD: return QString( "lod" ); break;
            case Property::SCALING: return QString( "scaling" ); break;
            case Property::FA_THRESHOLD: return QString( "fa threshold" ); break;
            case Property::EV_THRESHOLD: return QString( "ev threshold" ); break;
            case Property::GAMMA: return QString( "gamma" ); break;
            case Property::OFFSET: return QString( "offset" ); break;
            case Property::RENDER_SLICE: return QString( "render slice" ); break;
            case Property::RENDER_LOWER_X: return QString( "render lower x" ); break;
            case Property::RENDER_UPPER_X: return QString( "render upper x" ); break;
            case Property::RENDER_LOWER_Y: return QString( "render lower y" ); break;
            case Property::RENDER_UPPER_Y: return QString( "render upper y" ); break;
            case Property::RENDER_LOWER_Z: return QString( "render lower z" ); break;
            case Property::RENDER_UPPER_Z: return QString( "render upper z" ); break;
            case Property::MINMAX_SCALING: return QString( "min max scaling" ); break;
            case Property::BVALUE: return QString( "bvalue" ); break;
            case Property::TENSOR_RENDERMODE: return QString( "tensor render mode" ); break;
            case Property::ISO_VALUE: return QString( "iso value" ); break;
            case Property::NUM_POINTS: return QString( "num points" ); break;
            case Property::NUM_LINES: return QString( "num lines" ); break;
            case Property::FIBER_COLORMODE: return QString( "fiber color mode" ); break;
            case Property::FIBER_RENDERMODE: return QString( "fiber render mode" ); break;
            case Property::TEXTURE_GLUINT: return QString( "texture gluint" ); break;
            case Property::DATASET_POINTER: return QString( "dataset pointer" ); break;
            case Property::NEW_DATASET: return QString( "new dataset" ); break;
            case Property::DATASET_LIST: return QString( "dataset list" ); break;

            }
            return QString( "property not defined" );
        }
    };

    struct Global2String {
        static QString s( Global p )
        {
            switch( p )
            {
                // Settings
                case Global::LOCK_WIDGETS: return QString( "lock widgets" ); break;
                case Global::RENDER_CROSSHAIRS: return QString( "render crosshairs" ); break;
                // Global Settings for rendering
                case Global::SAGITTAL: return QString( "global sagittal" ); break;
                case Global::CORONAL: return QString( "global coronal" ); break;
                case Global::AXIAL: return QString( "" ); break;
                case Global::MAX_SAGITTAL: return QString( "" ); break;
                case Global::MAX_CORONAL: return QString( "" ); break;
                case Global::MAX_AXIAL: return QString( "" ); break;
                case Global::SLICE_DX: return QString( "" ); break;
                case Global::SLICE_DY: return QString( "" ); break;
                case Global::SLICE_DZ: return QString( "" ); break;
                case Global::CORONAL_AXIAL: return QString( "" ); break;
                case Global::SAGITTAL_AXIAL: return QString( "" ); break;
                case Global::SAGITTAL_CORONAL: return QString( "" ); break;
                case Global::LAST_PATH: return QString( "" ); break;
                case Global::SHOW_SAGITTAL: return QString( "" ); break;
                case Global::SHOW_CORONAL: return QString( "" ); break;
                case Global::SHOW_AXIAL: return QString( "" ); break;
                // settings that need a datastore signal to update widgets need to be inserted above this line
                case Global::ZOOM: return QString( "" ); break;
                case Global::MOVEX: return QString( "" ); break;
                case Global::MOVEY: return QString( "" ); break;
                case Global::BBX: return QString( "" ); break;
                case Global::BBY: return QString( "" ); break;
                case Global::VIEW: return QString( "" ); break;
            }
            return QString( "property not defined" );
        }
    };
}

Q_DECLARE_METATYPE( Fn::DatasetType )
Q_DECLARE_METATYPE( Fn::Colormap )
Q_DECLARE_METATYPE( Fn::Algo )
Q_DECLARE_METATYPE( Fn::Orient )
Q_DECLARE_METATYPE( Fn::Property )


#endif /* ENUMS_H_ */
