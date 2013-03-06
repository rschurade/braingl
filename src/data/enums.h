/*
 * enums.h
 *
 *  Created on: May 04, 2012
 *      Author: Ralph Schurade
 */

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

    enum class ColormapEnum : int
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
        TRACT_DENSITY,
        CUT_SELECTED_FIBERS
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
        SELECTED_MIN,
        SELECTED_MAX,
        LOWER_THRESHOLD,
        UPPER_THRESHOLD,
        COLORMAP,
        RENDER_COLORMAP,
        COLORMAP_ORIENT,
        COLORMAP_X,
        COLORMAP_Y,
        COLORMAP_DX,
        COLORMAP_DY,
        COLORMAP_TEXT_SIZE,
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
        RENDER_SAGITTAL,
        RENDER_CORONAL,
        RENDER_AXIAL,
        MINMAX_SCALING,
        BVALUE,
        TENSOR_RENDERMODE,
        RENDER_FIRST,
        RENDER_SECOND,
        RENDER_THIRD,
        ISO_VALUE,
        NUM_POINTS,
        NUM_LINES,
        FIBER_COLOR,
        FIBER_COLORMODE,
        FIBER_RENDERMODE,
        FIBER_TUBE_THICKNESS,
        HAS_TEXTURE,
        SELECTED_TEXTURE,
        TEXTURE_GLUINT,
        DATASET_POINTER,
        NEW_DATASET,
        DATASET_LIST
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
            case Property::SELECTED_MIN: return QString( "min" ); break;
            case Property::SELECTED_MAX: return QString( "max" ); break;
            case Property::LOWER_THRESHOLD: return QString( "lower threshold" ); break;
            case Property::UPPER_THRESHOLD: return QString( "upper threshold" ); break;
            case Property::COLORMAP: return QString( "colormap" ); break;
            case Property::RENDER_COLORMAP: return QString( "render colormap" ); break;
            case Property::COLORMAP_ORIENT: return QString( "colormap orient" ); break;
            case Property::COLORMAP_X: return QString( "colormap x position" ); break;
            case Property::COLORMAP_Y: return QString( "colormap y position" ); break;
            case Property::COLORMAP_DX: return QString( "colormap x size" ); break;
            case Property::COLORMAP_DY: return QString( "colormap y size" ); break;
            case Property::COLORMAP_TEXT_SIZE: return QString( "colormap text size" ); break;
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
            case Property::RENDER_SAGITTAL: return QString( "render sagittal" ); break;
            case Property::RENDER_CORONAL: return QString( "render coronal" ); break;
            case Property::RENDER_AXIAL: return QString( "render axial" ); break;
            case Property::MINMAX_SCALING: return QString( "min max scaling" ); break;
            case Property::BVALUE: return QString( "bvalue" ); break;
            case Property::TENSOR_RENDERMODE: return QString( "tensor render mode" ); break;
            case Property::RENDER_FIRST: return QString( "render first peak" ); break;
            case Property::RENDER_SECOND: return QString( "render second peak" ); break;
            case Property::RENDER_THIRD: return QString( "render third peak" ); break;
            case Property::ISO_VALUE: return QString( "iso value" ); break;
            case Property::NUM_POINTS: return QString( "num points" ); break;
            case Property::NUM_LINES: return QString( "num lines" ); break;
            case Property::FIBER_COLOR: return QString( "fiber color" ); break;
            case Property::FIBER_COLORMODE: return QString( "fiber color mode" ); break;
            case Property::FIBER_RENDERMODE: return QString( "fiber render mode" ); break;
            case Property::FIBER_TUBE_THICKNESS: return QString( "fiber tube thickness" ); break;
            case Property::HAS_TEXTURE: return QString( "has frame" ); break;
            case Property::SELECTED_TEXTURE: return QString( "selected frame" ); break;
            case Property::TEXTURE_GLUINT: return QString( "texture gluint" ); break;
            case Property::DATASET_POINTER: return QString( "dataset pointer" ); break;
            case Property::NEW_DATASET: return QString( "new dataset" ); break;
            case Property::DATASET_LIST: return QString( "dataset list" ); break;

            }
            return QString( "property not defined" );
        }
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
       SCREENSHOT_PATH,
       SHOW_SAGITTAL,
       SHOW_CORONAL,
       SHOW_AXIAL,
       BACKGROUND_COLOR_MAIN,
       BACKGROUND_COLOR_COMBINED,
       BACKGROUND_COLOR_NAV1,
       BACKGROUND_COLOR_NAV2,
       BACKGROUND_COLOR_NAV3,
       CROSSHAIR_COLOR,
       SHOW_NAV_SLIDERS,
       OBJECT,
   // settings that need a datastore signal to update widgets need to be inserted above this line
       ZOOM,
       MOVEX,
       MOVEY,
       BBX,
       BBY,
       VIEW,
       SCREENSHOT_QUALITY
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
                case Global::AXIAL: return QString( "global axial" ); break;
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
                case Global::SCREENSHOT_PATH: return QString( "screenshot dir" ); break;
                case Global::SHOW_SAGITTAL: return QString( "" ); break;
                case Global::SHOW_CORONAL: return QString( "" ); break;
                case Global::SHOW_AXIAL: return QString( "" ); break;
                case Global::BACKGROUND_COLOR_MAIN: return QString( "background color main" ); break;
                case Global::BACKGROUND_COLOR_COMBINED: return QString( "background color combined" ); break;
                case Global::BACKGROUND_COLOR_NAV1: return QString( "background color nav 1" ); break;
                case Global::BACKGROUND_COLOR_NAV2: return QString( "background color nav 2" ); break;
                case Global::BACKGROUND_COLOR_NAV3: return QString( "background color nav 3" ); break;
                case Global::CROSSHAIR_COLOR: return QString( "crosshair color" ); break;
                case Global::SHOW_NAV_SLIDERS: return QString( "show nav sliders" ); break;
                case Global::SCREENSHOT_QUALITY: return QString( "screenshot quality" ); break;
                case Global::OBJECT: return QString( "" ); break;
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

    enum class ROI : int
    {
        NAME,
        X,
        Y,
        Z,
        DX,
        DY,
        DZ,
        NEG,
        TYPE,
        ACTIVE,
        COLOR,
        RENDER,
        STICK_TO_CROSSHAIR,
        ID,
        PICK_ID,
        UPDATED,
        POINTER
    };

    struct ROI2String {
        static QString s( ROI p )
        {
            switch( p )
            {
                case ROI::NAME: return QString( "name" ); break;
                case ROI::X: return QString( "x" ); break;
                case ROI::Y: return QString( "y" ); break;
                case ROI::Z: return QString( "z" ); break;
                case ROI::DX: return QString( "dx" ); break;
                case ROI::DY: return QString( "dy" ); break;
                case ROI::DZ: return QString( "dz" ); break;
                case ROI::NEG: return QString( "neg" ); break;
                case ROI::TYPE: return QString( "type" ); break;
                case ROI::ACTIVE: return QString( "active" ); break;
                case ROI::COLOR: return QString( "color" ); break;
                case ROI::RENDER: return QString( "render" ); break;
                case ROI::STICK_TO_CROSSHAIR: return QString( "stick to crosshair" ); break;
                case ROI::ID: return QString( "id" ); break;
                case ROI::PICK_ID: return QString( "pick id" ); break;
                case ROI::UPDATED: return QString( "updated" ); break;
                case ROI::POINTER: return QString( "pointer" ); break;
            }
        return QString( "property not defined" );
        }
    };

    enum class ROIType : int
    {
        Box,
        Potato
    };
}

Q_DECLARE_METATYPE( Fn::DatasetType )
Q_DECLARE_METATYPE( Fn::ColormapEnum )
Q_DECLARE_METATYPE( Fn::Algo )
Q_DECLARE_METATYPE( Fn::Orient )
Q_DECLARE_METATYPE( Fn::Property )
Q_DECLARE_METATYPE( Fn::Global )


#endif /* ENUMS_H_ */
