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
        UNKNOWN = 0x00,
        NIFTI_SCALAR = 0x01,
        NIFTI_VECTOR = 0x02,
        NIFTI_TENSOR = 0x04,
        NIFTI_SH = 0x08,
        NIFTI_FMRI = 0x10,
        NIFTI_DWI = 0x20,
        NIFTI_BINGHAM = 0x40,
        NIFTI_ANY = 0xFF,
        MESH_ASCII = 0x100,
        MESH_BINARY = 0x200,
        MESH_ISOSURFACE = 0x400,
        MESH_TIME_SERIES = 0x800,
        MESH_CORRELATION = 0x1000,
        FIBERS = 0x2000,
        GLYPHSET = 0x4000,
        SURFACESET = 0x8000,
        CONS = 0x10000,
        TREE = 0x20000,
        CONGLYPHS = 0x40000,
        PLANE = 0x80000,
        ISO_LINE = 0x100000
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
        SD,
        FA,
        EV,
        BINGHAM,
        ISOSURFACE,
        ISOLINE,
        DISTANCE_MAP,
        GAUSS,
        MEDIAN,
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
        AVG_CON,
        AVG_CON_RZ,
        LITTLE_BRAINS,
        SH_2_MESH,
        LOOP_SUBDIVISION,
        MESH_TIME_SERIES,
        MESH_CORRELATION,
        MESH_BIGGEST_COMPONENT,
        MESH_DECIMATE,
        FIBER_DOWNSAMPLE,
        CONS_TO_GLYPHSET,
        FIBER_BUNDLING,
        CREATE_ROI,
        DELETE_LITTLE_BRAINS,
        COLOR_LITTLE_BRAINS,
        APPLY_ROI_BRAINS,
        BRAINGL_MATH
    };

    enum class Orient : int
    {
        NONE,
        AXIAL,
        CORONAL,
        SAGITTAL,
        AXIAL2,
        CORONAL2,
        SAGITTAL2
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
        D_RENDER_VECTORS_STIPPLES,
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
        D_LITTLE_BRAIN_VISIBILITY,
        D_IS_ATLAS,
        D_LOCK_PROPS,
        D_AUTOPLAY,
        D_AUTOPLAY_INTERVAL,
        D_MESH_CUT_LOWER_X,
        D_MESH_CUT_LOWER_Y,
        D_MESH_CUT_LOWER_Z,
        D_MESH_CUT_HIGHER_X,
        D_MESH_CUT_HIGHER_Y,
        D_MESH_CUT_HIGHER_Z,
        D_FIBER_GROW_LENGTH,
        D_ADJUST_X,
        D_ADJUST_Y,
        D_ADJUST_Z,
        D_GLYPHSET_PICKED_ID,
        D_GLYPH_COLORMODE,
        D_GLYPH_ROTATION,
        D_LIGHT_SWITCH,
        D_LIGHT_AMBIENT,
        D_LIGHT_DIFFUSE,
        D_LIGHT_SPECULAR,
        D_MATERIAL_AMBIENT,
        D_MATERIAL_DIFFUSE,
        D_MATERIAL_SPECULAR,
        D_MATERIAL_SHININESS,
        D_COPY_COLORS,
        D_RENDER_WIREFRAME,
        D_ROTATE_X,
        D_ROTATE_Y,
        D_ROTATE_Z,
        D_SCALE_X,
        D_SCALE_Y,
        D_SCALE_Z,
        D_MESH_MAKE_PERMANENT,
        D_MESH_NUM_VERTEX,
        D_MESH_NUM_TRIANGLES,
        D_TREE_SELECTED_CLUSTER,
        D_TREE_SELECTED_CLUSTER_COLOR,
        D_TREE_UNSELECTED_CLUSTER_COLOR,
        D_S_FORM,
        D_Q_FORM,
        D_USE_TRANSFORM,
        D_TRANSFORM,
        D_APPLY_TRANSFORM,
        D_INVERT_VERTEX_ORDER,
        D_START_INDEX,
        D_END_INDEX,
        D_LEFT_RIGHT,
        D_TREE_COLOR_SELECTION,
        D_TREE_USER_CLUSTER_COLOR,
        D_TREE_SET_USER_CLUSTER_COLOR,
        D_TREE_PARTITION_MODE,
        D_TREE_APPLY_PARTITION_MODE,
        D_TREE_PARTITION_LEVEL,
        D_TREE_PARTITION_SIZE,
        D_GLYPH_THRESHOLD_SIGN,
        D_LITTLE_BRAINS_COLORMODE,
        D_THRESHOLD_PERC,
        D_FIBER_MORPH,
        D_SHOW_PLANE_HANDLES,
        D_HANDLE_COLOR,
        D_LINE_WIDTH,
        D_STIPPLE_PROB_MASK,
        // Global Settings
        G_FIRST = 500, // insert all global properties after this one
        G_LOCK_WIDGETS,
        G_RENDER_CROSSHAIRS,
        // Global Settings for rendering
        G_SAGITTAL,
        G_CORONAL,
        G_AXIAL,
        G_CORONAL_AXIAL,
        G_SAGITTAL_AXIAL,
        G_SAGITTAL_CORONAL,
        G_SHOW_SAGITTAL,
        G_SHOW_CORONAL,
        G_SHOW_AXIAL,
        G_SHOW_NAV_SLIDERS,
        G_ZOOM,
        G_MOVEX,
        G_MOVEY,
        G_CAMERA_TYPE,
        G_CAMERA_PROJECTION,
        G_CAMERA_POS_X,
        G_CAMERA_POS_Y,
        G_CAMERA_POS_Z,
        G_CAMERA_LOOKAT_X,
        G_CAMERA_LOOKAT_Y,
        G_CAMERA_LOOKAT_Z,
        G_CAMERA_UP_X,
        G_CAMERA_UP_Y,
        G_CAMERA_UP_Z,
        G_CAMERA_NEAR,
        G_CAMERA_FAR,
        G_CAMERA_ANGLE,
        G_SCREENSHOT_QUALITY,
        G_TRANSPARENCY,
        G_LAST_PATH,
        G_SCREENSHOT_PATH,
        G_OBJECT,
        G_NEED_SHADER_UPDATE,
        G_BACKGROUND_COLOR_MAIN,
        G_BACKGROUND_COLOR_MAIN2,
        G_BACKGROUND_COLOR_COMBINED,
        G_BACKGROUND_COLOR_NAV1,
        G_BACKGROUND_COLOR_NAV2,
        G_BACKGROUND_COLOR_NAV3,
        G_CROSSHAIR_COLOR,
        G_SCREENSHOT_WIDTH,
        G_SCREENSHOT_HEIGHT,
        G_SCREENSHOT_KEEP_ASPECT,
        G_SCREENSHOT_SIZE_RATIO,
        G_SCREENSHOT_SIZE_SELECTION,
        G_SCREENSHOT_COPY_CURRENT,
        G_WIDTH_MAINGL,
        G_HEIGHT_MAINGL,
        G_WIDTH_MAINGL2,
        G_HEIGHT_MAINGL2,
        G_SCREENSHOT_DO_MAINGL,
        G_SCREENSHOT_DO_MAINGL2,
        G_SCREENSHOT_STEREOSCOPIC,
        G_SCREENSHOT_PREFIX,
        G_SCREENSHOT_PREFIX2,
        G_SCREENSHOT_DIGITS,
        G_SCREENSHOT_CURRENT_NUMBER,
        G_ARCBALL_DISTANCE,
        G_CAMERA_KEYBOARD_STEP,
        G_CAMERA_FIXATE_Z,
        G_CAMERA_FULLCIRCLE_STEPS,
        G_LIGHT_SWITCH,
        G_LIGHT_AMBIENT,
        G_LIGHT_DIFFUSE,
        G_FILTER_SIZE,
        G_DECIMATE_EPSILON,
        G_MIN_COMPONENT_SIZE,
        G_RENDER_CROSSHAIRS_2,
        G_UNSELECTED_FIBERS_GREY,
        G_TRACT_TEX_RESOLUTION,
        G_TRACT_TEXT_SOURCE,
        G_ISOLINE_STANDARD_COLOR,
        G_LAST, // insert all global properties before this one
        // ROI Properties
        D_X = 1000,
        D_Y,
        D_Z,
        D_NEG,
        D_RENDER,
        D_SHAPE,
        D_STICK_TO_CROSSHAIR,
        D_ID,
        D_PICK_ID,
        D_UPDATED,
        D_POINTER,
        D_DESCRIPTION
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
                case Property::D_RENDER_VECTORS_STIPPLES: return QString( "render vector stipples" ); break;
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
                case Property::D_LITTLE_BRAIN_VISIBILITY: return QString( "visibility of little brain glyphs" ); break;
                case Property::D_IS_ATLAS: return QString( "is atlas" ); break;
                case Property::D_LOCK_PROPS: return QString( "lock properties" ); break;
                case Property::D_AUTOPLAY: return QString( "auto play" ); break;
                case Property::D_AUTOPLAY_INTERVAL: return QString( "auto play interval" ); break;
                case Property::D_MESH_CUT_LOWER_X: return QString( "cut lower than x" ); break;
                case Property::D_MESH_CUT_LOWER_Y: return QString( "cut lower than y" ); break;
                case Property::D_MESH_CUT_LOWER_Z: return QString( "cut lower than z" ); break;
                case Property::D_MESH_CUT_HIGHER_X: return QString( "cut higher than x" ); break;
                case Property::D_MESH_CUT_HIGHER_Y: return QString( "cut higher than y" ); break;
                case Property::D_MESH_CUT_HIGHER_Z: return QString( "cut higher than z" ); break;
                case Property::D_FIBER_GROW_LENGTH: return QString( "fiber growth" ); break;
                case Property::D_ADJUST_X: return QString( "adjust x" ); break;
                case Property::D_ADJUST_Y: return QString( "adjust y" ); break;
                case Property::D_ADJUST_Z: return QString( "adjust z" ); break;
                case Property::D_GLYPHSET_PICKED_ID: return QString( "picked ID" ); break;
                case Property::D_GLYPH_COLORMODE: return QString( "colormode for glyphs" ); break;
                case Property::D_GLYPH_ROTATION: return QString( "glyph rotation enabled" ); break;
                case Property::D_LIGHT_SWITCH: return QString( "toggle light" ); break;
                case Property::D_LIGHT_AMBIENT: return QString( "light ambient" ); break;
                case Property::D_LIGHT_DIFFUSE: return QString( "light diffuse" ); break;
                case Property::D_LIGHT_SPECULAR: return QString( "light specular" ); break;
                case Property::D_MATERIAL_AMBIENT: return QString( "material ambient" ); break;
                case Property::D_MATERIAL_DIFFUSE: return QString( "material diffuse" ); break;
                case Property::D_MATERIAL_SPECULAR: return QString( "material specular" ); break;
                case Property::D_MATERIAL_SHININESS: return QString( "material shininess" ); break;
                case Property::D_COPY_COLORS: return QString( "copy colors" ); break;
                case Property::D_RENDER_WIREFRAME: return QString( "wireframe mode" ); break;
                case Property::D_ROTATE_X: return QString( "rotate x" ); break;
                case Property::D_ROTATE_Y: return QString( "rotate y" ); break;
                case Property::D_ROTATE_Z: return QString( "rotate z" ); break;
                case Property::D_SCALE_X: return QString( "scale x" ); break;
                case Property::D_SCALE_Y: return QString( "scale y" ); break;
                case Property::D_SCALE_Z: return QString( "scale z" ); break;
                case Property::D_MESH_MAKE_PERMANENT: return QString( "make permanent" ); break;
                case Property::D_MESH_NUM_VERTEX: return QString( "num vertexes" ); break;
                case Property::D_MESH_NUM_TRIANGLES: return QString( "num triangles" ); break;
                case Property::D_TREE_SELECTED_CLUSTER: return QString( "selected cluster" ); break;
                case Property::D_TREE_SELECTED_CLUSTER_COLOR: return QString( "selected cluster color" ); break;
                case Property::D_TREE_UNSELECTED_CLUSTER_COLOR: return QString( "unselected cluster color" ); break;
                case Property::D_S_FORM: return QString( "s form matrix" ); break;
                case Property::D_Q_FORM: return QString( "q form matrix" ); break;
                case Property::D_USE_TRANSFORM: return QString( "use transformation matrix" ); break;
                case Property::D_TRANSFORM: return QString( "transformation matrix" ); break;
                case Property::D_APPLY_TRANSFORM: return QString( "apply transformation" ); break;
                case Property::D_INVERT_VERTEX_ORDER: return QString( "invert vertex order" ); break;
                case Property::D_START_INDEX: return QString( "first index that gets rendered" ); break;
                case Property::D_END_INDEX: return QString( "last index that gets rendered" ); break;
                case Property::D_LEFT_RIGHT: return QString( "left right visibility" ); break;
                case Property::D_TREE_COLOR_SELECTION: return QString( "color mode" ); break;
                case Property::D_TREE_USER_CLUSTER_COLOR: return QString( "user defined cluster color" ); break;
                case Property::D_TREE_SET_USER_CLUSTER_COLOR: return QString( "set cluster color" ); break;
                case Property::D_TREE_PARTITION_MODE: return QString( "partition mode" ); break;
                case Property::D_TREE_APPLY_PARTITION_MODE: return QString( "apply partition mode" ); break;
                case Property::D_TREE_PARTITION_LEVEL: return QString( "partition level" ); break;
                case Property::D_TREE_PARTITION_SIZE: return QString( "partition size" ); break;
                case Property::D_GLYPH_THRESHOLD_SIGN: return QString( "threshold interpretation" ); break;
                case Property::D_LITTLE_BRAINS_COLORMODE: return QString( "little brains color mode" ); break;
                case Property::D_THRESHOLD_PERC: return QString( "threshold as percentage" ); break;
                case Property::D_FIBER_MORPH: return QString( "interpolation btw. straight line and data" ); break;
                case Property::D_SHOW_PLANE_HANDLES: return QString( "show/hide handles" ); break;
                case Property::D_HANDLE_COLOR: return QString( "handle color" ); break;
                case Property::D_LINE_WIDTH: return QString( "line width" ); break;
                case Property::D_STIPPLE_PROB_MASK: return QString( "probability mask" ); break;
                // Global Settings
                case Property::G_FIRST: return QString( "placeholder global first" ); break;
                case Property::G_LOCK_WIDGETS: return QString( "lock widgets" ); break;
                case Property::G_RENDER_CROSSHAIRS: return QString( "render crosshairs" ); break; // Global Settings for rendering
                case Property::G_SAGITTAL: return QString( "global sagittal" ); break;
                case Property::G_CORONAL: return QString( "global coronal" ); break;
                case Property::G_AXIAL: return QString( "global axial" ); break;
                case Property::G_CORONAL_AXIAL: return QString( "" ); break;
                case Property::G_SAGITTAL_AXIAL: return QString( "" ); break;
                case Property::G_SAGITTAL_CORONAL: return QString( "" ); break;
                case Property::G_SHOW_SAGITTAL: return QString( "show sagittal" ); break;
                case Property::G_SHOW_CORONAL: return QString( "show coronal" ); break;
                case Property::G_SHOW_AXIAL: return QString( "show axial" ); break;
                case Property::G_SHOW_NAV_SLIDERS: return QString( "show nav sliders" ); break;
                case Property::G_SCREENSHOT_QUALITY: return QString( "screenshot quality" ); break;
                case Property::G_TRANSPARENCY: return QString( "transparency mode" ); break;
                case Property::G_NEED_SHADER_UPDATE: return QString( "" ); break;
                case Property::G_ZOOM: return QString( "" ); break;
                case Property::G_MOVEX: return QString( "" ); break;
                case Property::G_MOVEY: return QString( "" ); break;
                case Property::G_CAMERA_TYPE: return QString( "camera type" ); break;
                case Property::G_CAMERA_PROJECTION: return QString( "camera projection" ); break;
                case Property::G_CAMERA_POS_X: return QString( "eye x" ); break;
                case Property::G_CAMERA_POS_Y: return QString( "eye y" ); break;
                case Property::G_CAMERA_POS_Z: return QString( "eye z" ); break;
                case Property::G_CAMERA_LOOKAT_X: return QString( "look at x" ); break;
                case Property::G_CAMERA_LOOKAT_Y: return QString( "look at y" ); break;
                case Property::G_CAMERA_LOOKAT_Z: return QString( "look at z" ); break;
                case Property::G_CAMERA_UP_X: return QString( "up x" ); break;
                case Property::G_CAMERA_UP_Y: return QString( "up y" ); break;
                case Property::G_CAMERA_UP_Z: return QString( "up z" ); break;
                case Property::G_CAMERA_NEAR: return QString( "camera near" ); break;
                case Property::G_CAMERA_FAR: return QString( "camera far" ); break;
                case Property::G_CAMERA_ANGLE: return QString( "camera angle" ); break;
                case Property::G_OBJECT: return QString( "" ); break;
                case Property::G_LAST_PATH: return QString( "" ); break;
                case Property::G_SCREENSHOT_PATH: return QString( "screenshot dir" ); break;
                case Property::G_BACKGROUND_COLOR_MAIN: return QString( "background color main" ); break;
                case Property::G_BACKGROUND_COLOR_MAIN2: return QString( "background color main 2" ); break;
                case Property::G_BACKGROUND_COLOR_COMBINED: return QString( "background color combined" ); break;
                case Property::G_BACKGROUND_COLOR_NAV1: return QString( "background color nav 1" ); break;
                case Property::G_BACKGROUND_COLOR_NAV2: return QString( "background color nav 2" ); break;
                case Property::G_BACKGROUND_COLOR_NAV3: return QString( "background color nav 3" ); break;
                case Property::G_CROSSHAIR_COLOR: return QString( "crosshair color" ); break;
                case Property::G_SCREENSHOT_WIDTH: return QString( "screenshot width" ); break;
                case Property::G_SCREENSHOT_HEIGHT: return QString( "screenshot height" ); break;
                case Property::G_SCREENSHOT_KEEP_ASPECT: return QString( "keep current aspect ratio" ); break;
                case Property::G_SCREENSHOT_SIZE_RATIO: return QString( "current aspect ratio" ); break;
                case Property::G_SCREENSHOT_SIZE_SELECTION: return QString( "select screenshot size" ); break;
                case Property::G_SCREENSHOT_COPY_CURRENT: return QString( "copy current screen size" ); break;
                case Property::G_WIDTH_MAINGL: return QString( "width maingl" ); break;
                case Property::G_HEIGHT_MAINGL: return QString( "height maingl" ); break;
                case Property::G_WIDTH_MAINGL2: return QString( "width maingl2" ); break;
                case Property::G_HEIGHT_MAINGL2: return QString( "width maingl2" ); break;
                case Property::G_SCREENSHOT_DO_MAINGL: return QString( "do maingl" ); break;
                case Property::G_SCREENSHOT_DO_MAINGL2: return QString( "do maingl2" ); break;
                case Property::G_SCREENSHOT_STEREOSCOPIC: return QString( "stereoscopic screenshot" ); break;
                case Property::G_SCREENSHOT_PREFIX: return QString( "filename prefix" ); break;
                case Property::G_SCREENSHOT_PREFIX2: return QString( "filename prefix maingl2" ); break;
                case Property::G_SCREENSHOT_DIGITS: return QString( "filename num digits" ); break;
                case Property::G_SCREENSHOT_CURRENT_NUMBER: return QString( "current screenshot number" ); break;
                case Property::G_ARCBALL_DISTANCE: return QString( "arcball distance" ); break;
                case Property::G_CAMERA_KEYBOARD_STEP: return QString( "keyboard step size" ); break;
                case Property::G_CAMERA_FIXATE_Z: return QString( "fixate z" ); break;
                case Property::G_CAMERA_FULLCIRCLE_STEPS: return QString( "steps to do full cirlce" ); break;
                case Property::G_LIGHT_SWITCH: return QString( "toggle light" ); break;
                case Property::G_LIGHT_AMBIENT: return QString( "light ambient" ); break;
                case Property::G_LIGHT_DIFFUSE: return QString( "light diffuse" ); break;
                case Property::G_FILTER_SIZE: return QString( "filter size (voxel)" ); break;
                case Property::G_DECIMATE_EPSILON: return QString( "epsilon for decimate" ); break;
                case Property::G_MIN_COMPONENT_SIZE: return QString( "min size for component extraction" ); break;
                case Property::G_RENDER_CROSSHAIRS_2: return QString( "render crosshairs maingl2" ); break;
                case Property::G_UNSELECTED_FIBERS_GREY: return QString( "render hidden fibers grey" ); break;
                case Property::G_TRACT_TEX_RESOLUTION: return QString( "resolution of tract textures" ); break;
                case Property::G_TRACT_TEXT_SOURCE: return QString( "source for tract textures" ); break;
                case Property::G_ISOLINE_STANDARD_COLOR: return QString( "isoline standard color" ); break;
                case Property::G_LAST: return QString( "placeholder global last" ); break;
                // ROI Properties
                case Property::D_X: return QString( "x" ); break;
                case Property::D_Y: return QString( "y" ); break;
                case Property::D_Z: return QString( "z" ); break;
                case Property::D_NEG: return QString( "negate" ); break;
                case Property::D_RENDER: return QString( "render" ); break;
                case Property::D_SHAPE: return QString( "shape" ); break;
                case Property::D_STICK_TO_CROSSHAIR: return QString( "stick to crosshair" ); break;
                case Property::D_ID: return QString( "id" ); break;
                case Property::D_PICK_ID: return QString( "pick id" ); break;
                case Property::D_UPDATED: return QString( "updated" ); break;
                case Property::D_POINTER: return QString( "pointer" ); break;
                case Property::D_DESCRIPTION: return QString( "description" ); break;
            }
            return QString( "property not defined" );
        }
    };

    enum class ROIType : int
    {
        Box, Sphere, Potato
    };

    enum class Resolution : int
    {
        R_1024x768,
        R_1920x1080,
        R_1920x1200
    };

    struct Resolution2String
    {
        static QString s( Resolution r )
        {
            switch ( r )
            {
                case Resolution::R_1024x768: return QString( "1024x786" ); break;
                case Resolution::R_1920x1080: return QString( "1920x1080" ); break;
                case Resolution::R_1920x1200: return QString( "1920x1200" ); break;
            }
            return QString( "resolution not defined" );
        }
    };
}

Q_DECLARE_METATYPE( Fn::DatasetType )
Q_DECLARE_METATYPE( Fn::ColormapEnum )
Q_DECLARE_METATYPE( Fn::Algo )
Q_DECLARE_METATYPE( Fn::Orient )
Q_DECLARE_METATYPE( Fn::Property )

#endif /* ENUMS_H_ */
