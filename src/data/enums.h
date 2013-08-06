/*
 * enums.h
 *
 * Created on: May 04, 2012
 * @author Ralph Schurade
 */

#ifndef ENUMS_H_
#define ENUMS_H_

#include <QMetaType>

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
        FIBERS,
        GLYPHSET,
        CONS
    };

    enum class ColormapEnum : int
    {
        GRAY, RAINBOW1, RAINBOW2, BLUEWHITERED, NONE // used for RGB datasets, takes values directly from texture                                             // other colormaps use float packing
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
        DISTANCE_MAP,
        TENSOR_TRACK,
        CROSSING_TRACK,
        BINGHAM_2_TENSOR,
        FIBER_THINNING,
        TRACT_DENSITY,
        TRACT_COLOR,
        CUT_SELECTED_FIBERS,
        CREATE_NEW_DATASET,
        MAKECONS,
        BUNDLE,
        SAVERGB,
        LOADRGB,
        SAVE1D,
        LOAD1D,
        AVG_CON,
        AVG_CON_RZ
    };

    enum class Orient : int
    {
        NONE, AXIAL, CORONAL, SAGITTAL
    };

    enum class Position : int
    {
        CENTER = 0x00,
        NORTH = 0x01,
        NORTH_EAST = 0x02,
        EAST = 0x04,
        SOUTH_EAST = 0x08,
        SOUTH = 0x10,
        SOUTH_WEST = 0x11,
        WEST = 0x12,
        NORTH_WEST = 0x14
    };

    enum class Property : int
    {
        // dataset properties
        D_NAME, //name that is shown in the dataset list widget
        D_DIM,
        D_DATATYPE,
        D_SIZE,
        D_NX,
        D_NY,
        D_NZ,
        D_DX,
        D_DY,
        D_DZ,
        D_MIN,
        D_MAX,
        D_TYPE,
        D_CREATED_BY,
        D_FILENAME,
        D_SELECTED_MIN,
        D_SELECTED_MAX,
        D_LOWER_THRESHOLD,
        D_UPPER_THRESHOLD,
        D_COLORMAP,
        D_RENDER_COLORMAP,
        D_COLORMAP_ORIENT,
        D_COLORMAP_X,
        D_COLORMAP_Y,
        D_COLORMAP_DX,
        D_COLORMAP_DY,
        D_COLORMAP_TEXT_SIZE,
        D_COLORMAP_TEXT_COLOR,
        D_INTERPOLATION,
        D_ALPHA,
        D_ACTIVE,
        D_ORDER,
        D_LOD,
        D_SCALING,
        D_FA_THRESHOLD,
        D_EV_THRESHOLD,
        D_GAMMA,
        D_OFFSET,
        D_RENDER_SLICE,
        D_RENDER_SAGITTAL,
        D_RENDER_CORONAL,
        D_RENDER_AXIAL,
        D_MINMAX_SCALING,
        D_HIDE_NEGATIVE_LOBES,
        D_BVALUE,
        D_TENSOR_RENDERMODE,
        D_RENDER_FIRST,
        D_RENDER_SECOND,
        D_RENDER_THIRD,
        D_RENDER_VECTORS_STICKS,
        D_ISO_VALUE,
        D_NUM_POINTS,
        D_NUM_LINES,
        D_COLOR,
        D_COLORMODE,
        D_DATAMODE,
        D_PAINTMODE,
        D_PAINTSIZE,
        D_PAINTCOLOR,
        D_PAINTVALUE,
        D_PAINT_LINK_CURSOR,
        D_FIBER_RENDERMODE,
        D_FIBER_THICKNESS,
        D_HAS_TEXTURE,
        D_SELECTED_TEXTURE,
        D_TEXTURE_GLUINT,
        D_DATASET_POINTER,
        D_NEW_DATASET,
        D_DATASET_LIST,
        D_SURFACE,
        D_THRESHOLD,
        D_GLYPHSTYLE,
        D_GLYPHRADIUS,
        D_NORMALIZATION,
        D_PRIMSIZE,
        D_MINLENGTH,
        D_DRAW_SURFACE,
        D_DRAW_GLYPHS,
        D_SURFACE_GLYPH_GEOMETRY,
        D_SURFACE_GLYPH_COLOR,
        D_RENDER_TARGET,
        D_GLYPH_ROT_X,
        D_GLYPH_ROT_Y,
        D_GLYPH_ROT_Z,
        D_GLYPH_ALPHA,
        D_IS_ATLAS,
        D_DELETE_LAST,
        // Global Settings
        G_LOCK_WIDGETS,
        G_RENDER_CROSSHAIRS,
        // Global Settings for rendering
        G_SAGITTAL,
        G_CORONAL,
        G_AXIAL,
        G_MAX_SAGITTAL,
        G_MAX_CORONAL,
        G_MAX_AXIAL,
        G_SLICE_DX,
        G_SLICE_DY,
        G_SLICE_DZ,
        G_CORONAL_AXIAL,
        G_SAGITTAL_AXIAL,
        G_SAGITTAL_CORONAL,
        G_LAST_PATH,
        G_SCREENSHOT_PATH,
        G_SHOW_SAGITTAL,
        G_SHOW_CORONAL,
        G_SHOW_AXIAL,
        G_BACKGROUND_COLOR_MAIN,
        G_BACKGROUND_COLOR_COMBINED,
        G_BACKGROUND_COLOR_NAV1,
        G_BACKGROUND_COLOR_NAV2,
        G_BACKGROUND_COLOR_NAV3,
        G_CROSSHAIR_COLOR,
        G_SHOW_NAV_SLIDERS,
        G_OBJECT,
        G_NEED_SHADER_UPDATE,
        G_ZOOM,
        G_MOVEX,
        G_MOVEY,
        G_SCREENSHOT_QUALITY,
        G_TRANSPARENCY,
        // ROI Properties
        R_X,
        R_Y,
        R_Z,
        R_NX,
        R_NY,
        R_NZ,
        R_DX,
        R_DY,
        R_DZ,
        R_NEG,
        R_TYPE,
        R_COLOR,
        R_ALPHA,
        R_RENDER,
        R_SHAPE,
        R_STICK_TO_CROSSHAIR,
        R_ID,
        R_PICK_ID,
        R_UPDATED,
        R_THRESHOLD,
        R_POINTER
    };

    struct Prop2String
    {
        static QString s( Property p )
        {
            switch ( p )
            {
                case Property::D_NAME: return QString( "name" ); break;
                case Property::D_DIM: return QString( "dim" ); break;
                case Property::D_DATATYPE: return QString( "data type" ); break;
                case Property::D_SIZE: return QString( "size" ); break;
                case Property::D_NX: return QString( "nx" ); break;
                case Property::D_NY: return QString( "ny" ); break;
                case Property::D_NZ: return QString( "nz" ); break;
                case Property::D_DX: return QString( "dx" ); break;
                case Property::D_DY: return QString( "dy" ); break;
                case Property::D_DZ: return QString( "dz" ); break;
                case Property::D_MIN: return QString( "min" ); break;
                case Property::D_MAX: return QString( "max" ); break;
                case Property::D_TYPE: return QString( "type" ); break;
                case Property::D_CREATED_BY: return QString( "created by" ); break;
                case Property::D_FILENAME: return QString( "filename" ); break;
                case Property::D_SELECTED_MIN: return QString( "min" ); break;
                case Property::D_SELECTED_MAX: return QString( "max" ); break;
                case Property::D_LOWER_THRESHOLD: return QString( "lower threshold" ); break;
                case Property::D_UPPER_THRESHOLD: return QString( "upper threshold" ); break;
                case Property::D_COLORMAP: return QString( "colormap" ); break;
                case Property::D_RENDER_COLORMAP: return QString( "render colormap" ); break;
                case Property::D_COLORMAP_ORIENT: return QString( "colormap orient" ); break;
                case Property::D_COLORMAP_X: return QString( "colormap x position" ); break;
                case Property::D_COLORMAP_Y: return QString( "colormap y position" ); break;
                case Property::D_COLORMAP_DX: return QString( "colormap x size" ); break;
                case Property::D_COLORMAP_DY: return QString( "colormap y size" ); break;
                case Property::D_COLORMAP_TEXT_SIZE: return QString( "colormap text size" ); break;
                case Property::D_COLORMAP_TEXT_COLOR: return QString( "colormap text color" ); break;
                case Property::D_INTERPOLATION: return QString( "interpolation" ); break;
                case Property::D_ALPHA: return QString( "alpha" ); break;
                case Property::D_ACTIVE: return QString( "active" ); break;
                case Property::D_ORDER: return QString( "order" ); break;
                case Property::D_LOD: return QString( "lod" ); break;
                case Property::D_SCALING: return QString( "scaling" ); break;
                case Property::D_FA_THRESHOLD: return QString( "fa threshold" ); break;
                case Property::D_EV_THRESHOLD: return QString( "ev threshold" ); break;
                case Property::D_GAMMA: return QString( "gamma" ); break;
                case Property::D_OFFSET: return QString( "offset" ); break;
                case Property::D_RENDER_SLICE: return QString( "render slice" ); break;
                case Property::D_RENDER_SAGITTAL: return QString( "render sagittal" ); break;
                case Property::D_RENDER_CORONAL: return QString( "render coronal" ); break;
                case Property::D_RENDER_AXIAL: return QString( "render axial" ); break;
                case Property::D_MINMAX_SCALING: return QString( "min max scaling" ); break;
                case Property::D_HIDE_NEGATIVE_LOBES: return QString( "hide negative lobes" ); break;
                case Property::D_BVALUE: return QString( "bvalue" ); break;
                case Property::D_TENSOR_RENDERMODE: return QString( "tensor render mode" ); break;
                case Property::D_RENDER_FIRST: return QString( "render first peak" ); break;
                case Property::D_RENDER_SECOND: return QString( "render second peak" ); break;
                case Property::D_RENDER_THIRD: return QString( "render third peak" ); break;
                case Property::D_RENDER_VECTORS_STICKS: return QString( "render vector sticks" ); break;
                case Property::D_ISO_VALUE: return QString( "iso value" ); break;
                case Property::D_NUM_POINTS: return QString( "num points" ); break;
                case Property::D_NUM_LINES: return QString( "num lines" ); break;
                case Property::D_COLOR: return QString( "color" ); break;
                case Property::D_COLORMODE: return QString( "color mode" ); break;
                case Property::D_DATAMODE: return QString( "select data" ); break;
                case Property::D_PAINTMODE: return QString( "paint mode" ); break;
                case Property::D_PAINTSIZE: return QString( "brush size" ); break;
                case Property::D_PAINTCOLOR: return QString( "paint color" ); break;
                case Property::D_PAINTVALUE: return QString( "paint value" ); break;
                case Property::D_PAINT_LINK_CURSOR: return QString( "link cursor" ); break;
                case Property::D_FIBER_RENDERMODE: return QString( "fiber render mode" ); break;
                case Property::D_FIBER_THICKNESS: return QString( "fiber thickness" ); break;
                case Property::D_HAS_TEXTURE: return QString( "has frame" ); break;
                case Property::D_SELECTED_TEXTURE: return QString( "selected frame" ); break;
                case Property::D_TEXTURE_GLUINT: return QString( "texture gluint" ); break;
                case Property::D_DATASET_POINTER: return QString( "dataset pointer" ); break;
                case Property::D_NEW_DATASET: return QString( "new dataset" ); break;
                case Property::D_DATASET_LIST: return QString( "dataset list" ); break;
                case Property::D_SURFACE: return QString( "surface" ); break;
                case Property::D_THRESHOLD: return QString( "threshold" ); break;
                case Property::D_GLYPHSTYLE: return QString( "glyph style" ); break;
                case Property::D_GLYPHRADIUS: return QString( "scale" ); break;
                case Property::D_NORMALIZATION: return QString( "pie normalization" ); break;
                case Property::D_PRIMSIZE: return QString( "points / vector primitive size" ); break;
                case Property::D_MINLENGTH: return QString( "minimum length" ); break;
                case Property::D_DRAW_SURFACE: return QString( "surface visibility" ); break;
                case Property::D_DRAW_GLYPHS: return QString( "glyph visibility" ); break;
                case Property::D_SURFACE_GLYPH_GEOMETRY: return QString( "glyph geometry" ); break;
                case Property::D_SURFACE_GLYPH_COLOR: return QString( "glyph color" ); break;
                case Property::D_RENDER_TARGET: return QString( "internal: render target" ); break;
                case Property::D_GLYPH_ROT_X: return QString( "glyph x rotation" ); break;
                case Property::D_GLYPH_ROT_Y: return QString( "glyph y rotation" ); break;
                case Property::D_GLYPH_ROT_Z: return QString( "glyph z rotation" ); break;
                case Property::D_GLYPH_ALPHA: return QString( "glyph alpha" ); break;
                case Property::D_IS_ATLAS: return QString( "is atlas" ); break;
                case Property::D_DELETE_LAST: return QString( "xxx" ); break;
                // Global Settings
                case Property::G_LOCK_WIDGETS: return QString( "lock widgets" ); break;
                case Property::G_RENDER_CROSSHAIRS: return QString( "render crosshairs" ); break; // Global Settings for rendering
                case Property::G_SAGITTAL: return QString( "global sagittal" ); break;
                case Property::G_CORONAL: return QString( "global coronal" ); break;
                case Property::G_AXIAL: return QString( "global axial" ); break;
                case Property::G_MAX_SAGITTAL: return QString( "" ); break;
                case Property::G_MAX_CORONAL: return QString( "" ); break;
                case Property::G_MAX_AXIAL: return QString( "" ); break;
                case Property::G_SLICE_DX: return QString( "" ); break;
                case Property::G_SLICE_DY: return QString( "" ); break;
                case Property::G_SLICE_DZ: return QString( "" ); break;
                case Property::G_CORONAL_AXIAL: return QString( "" ); break;
                case Property::G_SAGITTAL_AXIAL: return QString( "" ); break;
                case Property::G_SAGITTAL_CORONAL: return QString( "" ); break;
                case Property::G_LAST_PATH: return QString( "" ); break;
                case Property::G_SCREENSHOT_PATH: return QString( "screenshot dir" ); break;
                case Property::G_SHOW_SAGITTAL: return QString( "show sagittal" ); break;
                case Property::G_SHOW_CORONAL: return QString( "show coronal" ); break;
                case Property::G_SHOW_AXIAL: return QString( "show axial" ); break;
                case Property::G_BACKGROUND_COLOR_MAIN: return QString( "background color main" ); break;
                case Property::G_BACKGROUND_COLOR_COMBINED: return QString( "background color combined" ); break;
                case Property::G_BACKGROUND_COLOR_NAV1: return QString( "background color nav 1" ); break;
                case Property::G_BACKGROUND_COLOR_NAV2: return QString( "background color nav 2" ); break;
                case Property::G_BACKGROUND_COLOR_NAV3: return QString( "background color nav 3" ); break;
                case Property::G_CROSSHAIR_COLOR: return QString( "crosshair color" ); break;
                case Property::G_SHOW_NAV_SLIDERS: return QString( "show nav sliders" ); break;
                case Property::G_SCREENSHOT_QUALITY: return QString( "screenshot quality" ); break;
                case Property::G_TRANSPARENCY: return QString( "transparency mode" ); break;
                case Property::G_OBJECT: return QString( "" ); break;
                case Property::G_NEED_SHADER_UPDATE: return QString( "" ); break;
                // settings that need a datastore signal to update widgets need to be inserted above this line
                case Property::G_ZOOM: return QString( "" ); break;
                case Property::G_MOVEX: return QString( "" ); break;
                case Property::G_MOVEY: return QString( "" ); break;
                // ROI Properties
                case Property::R_X: return QString( "x" ); break;
                case Property::R_Y: return QString( "y" ); break;
                case Property::R_Z: return QString( "z" ); break;
                case Property::R_NX: return QString( "nx" ); break;
                case Property::R_NY: return QString( "ny" ); break;
                case Property::R_NZ: return QString( "nz" ); break;
                case Property::R_DX: return QString( "dx" ); break;
                case Property::R_DY: return QString( "dy" ); break;
                case Property::R_DZ: return QString( "dz" ); break;
                case Property::R_NEG: return QString( "negate" ); break;
                case Property::R_TYPE: return QString( "type" ); break;
                case Property::R_COLOR: return QString( "color" ); break;
                case Property::R_ALPHA: return QString( "alpha" ); break;
                case Property::R_RENDER: return QString( "render" ); break;
                case Property::R_SHAPE: return QString( "shape" ); break;
                case Property::R_STICK_TO_CROSSHAIR: return QString( "stick to crosshair" ); break;
                case Property::R_ID: return QString( "id" ); break;
                case Property::R_PICK_ID: return QString( "pick id" ); break;
                case Property::R_UPDATED: return QString( "updated" ); break;
                case Property::R_THRESHOLD: return QString( "threshold" ); break;
                case Property::R_POINTER: return QString( "pointer" ); break;
            }
            return QString( "property not defined" );
        }
    };

    enum class ROIType : int
    {
        Box, Sphere, Potato
    };
}

Q_DECLARE_METATYPE( Fn::DatasetType )
Q_DECLARE_METATYPE( Fn::ColormapEnum )
Q_DECLARE_METATYPE( Fn::Algo )
Q_DECLARE_METATYPE( Fn::Orient )
Q_DECLARE_METATYPE( Fn::Property )

#endif /* ENUMS_H_ */
