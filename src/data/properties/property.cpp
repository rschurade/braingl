/*
 * property.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */

#include "property.h"

Property::Property( FN_PROPERTY name ) :
    m_name( name ),
    m_label( getNameAsString( name ) )
{
}

Property::~Property()
{
}

FN_PROPERTY Property::getName()
{
    return m_name;
}

QString Property::getLabel()
{
    return m_label;
}

QString Property::getNameAsString( FN_PROPERTY name )
{
    switch ( name )
    {
        case FNPROP_NAME:
            return "name";
            break;
        case FNPROP_DIM:
            return "dim";
            break;
        case FNPROP_DATATYPE:
            return "data type";
            break;
        case FNPROP_SIZE:
            return "size";
            break;
        case FNPROP_NX:
            return "nx";
            break;
        case FNPROP_NY:
            return "ny";
            break;
        case FNPROP_NZ:
            return "nz";
            break;
        case FNPROP_DX:
            return "dx";
            break;
        case FNPROP_DY:
            return "dy";
            break;
        case FNPROP_DZ:
            return "dz";
            break;
        case FNPROP_MIN:
            return "min";
            break;
        case FNPROP_MAX:
            return "max";
            break;
        case FNPROP_TYPE:
            return "type";
            break;
        case FNPROP_CREATED_BY:
            return "created by";
            break;
        case FNPROP_FILENAME:
            return "file name";
            break;
        case FNPROP_LOWER_THRESHOLD:
            return "lower threshold";
            break;
        case FNPROP_UPPER_THRESHOLD:
            return "upper threshold";
            break;
        case FNPROP_COLORMAP:
            return "colormap";
            break;
        case FNPROP_INTERPOLATION:
            return "interpolation";
            break;
        case FNPROP_ALPHA:
            return "alpha";
            break;
        case FNPROP_ACTIVE:
            return "active";
            break;
        case FNPROP_ORDER:
            return "order";
            break;
        case FNPROP_LOD:
            return "level of detail";
            break;
        case FNPROP_SCALING:
            return "scaling";
            break;
        case FNPROP_FA_THRESHOLD:
            return "FA threshold";
            break;
        case FNPROP_EV_THRESHOLD:
            return "EV threshold";
            break;
        case FNPROP_GAMMA:
            return "gamma";
            break;
        case FNPROP_OFFSET:
            return "offset";
            break;
        case FNPROP_RENDER_SLICE:
            return "render slice";
            break;
        case FNPROP_RENDER_LOWER_X:
            return "render lower x";
            break;
        case FNPROP_RENDER_UPPER_X:
            return "render upper x";
            break;
        case FNPROP_RENDER_LOWER_Y:
            return "render lower y";
            break;
        case FNPROP_RENDER_UPPER_Y:
            return "render upper y";
            break;
        case FNPROP_RENDER_LOWER_Z:
            return "render lower z";
            break;
        case FNPROP_RENDER_UPPER_Z:
            return "render upper z";
            break;
        case FNPROP_MINMAX_SCALING:
            return "minmax scaling";
            break;
        case FNPROP_BVALUE:
            return "bvalue";
            break;
        case FNPROP_CALC_LOWER_X:
            return "calc lower x";
            break;
        case FNPROP_CALC_UPPER_X:
            return "calc upper x";
            break;
        case FNPROP_CALC_LOWER_Y:
            return "calc lower y";
            break;
        case FNPROP_CALC_UPPER_Y:
            return "calc upper y";
            break;
        case FNPROP_CALC_LOWER_Z:
            return "calc lower z";
            break;
        case FNPROP_CALC_UPPER_Z:
            return "calc upper z";
            break;
        case FNPROP_TENSOR_RENDERMODE:
            return "tensor render moder";
            break;
        case FNPROP_ISO_VALUE:
            return "iso value";
            break;
/*        case FNPROP_SAGITTAL:
            return "sagittal";
            break;
        case FNPROP_CORONAL:
            return "coronal";
            break;
        case FNPROP_AXIAL:
            return "axial";
            break;
        case FNPROP_MAX_SAGITTAL:
            return "max saggital";
            break;
        case FNPROP_MAX_CORONAL:
            return "max coronal";
            break;
        case FNPROP_MAX_AXIAL:
            return "max axial";
            break;
        case FNPROP_SLICE_DX:
            return "slice dx";
            break;
        case FNPROP_SLICE_DY:
            return "slice dy";
            break;
        case FNPROP_SLICE_DZ:
            return "slice dz";
            break;
        case FNPROP_LAST_PATH:
            return "last path";
            break;
        case FNPROP_SHOW_SAGITTAL:
            return "show sagittal";
            break;
        case FNPROP_SHOW_CORONAL:
            return "show coronal";
            break;
        case FNPROP_SHOW_AXIAL:
            return "show axial";
            break;
        case FNPROP_ZOOM:
            return "zoom";
            break;
        case FNPROP_MOVEX:
            return "move x";
            break;
        case FNPROP_MOVEY:
            return "move y";
            break;
        case FNPROP_BBX:
            return "bounding box x";
            break;
        case FNPROP_BBY:
            return "bounding box y";
            break;
        case FNPROP_VIEW:
            return "view";
            break; */
        default:
            return "no string set for property";
            break;
    }
    return "";
}
