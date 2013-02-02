/*
 * property.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "property.h"

Property::Property( Fn::Property name ) :
    m_name( name ),
    m_label( getNameAsString( name ) )
{
}

Property::~Property()
{
}

Fn::Property Property::getName()
{
    return m_name;
}

QString Property::getLabel()
{
    return m_label;
}

QString Property::getNameAsString( Fn::Property name )
{
    switch ( name )
    {
        case Fn::Property::NAME:
            return "name";
            break;
        case Fn::Property::DIM:
            return "dim";
            break;
        case Fn::Property::DATATYPE:
            return "data type";
            break;
        case Fn::Property::SIZE:
            return "size";
            break;
        case Fn::Property::NX:
            return "nx";
            break;
        case Fn::Property::NY:
            return "ny";
            break;
        case Fn::Property::NZ:
            return "nz";
            break;
        case Fn::Property::DX:
            return "dx";
            break;
        case Fn::Property::DY:
            return "dy";
            break;
        case Fn::Property::DZ:
            return "dz";
            break;
        case Fn::Property::MIN:
            return "min";
            break;
        case Fn::Property::MAX:
            return "max";
            break;
        case Fn::Property::TYPE:
            return "type";
            break;
        case Fn::Property::CREATED_BY:
            return "created by";
            break;
        case Fn::Property::FILENAME:
            return "file name";
            break;
        case Fn::Property::LOWER_THRESHOLD:
            return "lower threshold";
            break;
        case Fn::Property::UPPER_THRESHOLD:
            return "upper threshold";
            break;
        case Fn::Property::COLORMAP:
            return "colormap";
            break;
        case Fn::Property::INTERPOLATION:
            return "interpolation";
            break;
        case Fn::Property::ALPHA:
            return "alpha";
            break;
        case Fn::Property::ACTIVE:
            return "active";
            break;
        case Fn::Property::ORDER:
            return "order";
            break;
        case Fn::Property::LOD:
            return "level of detail";
            break;
        case Fn::Property::SCALING:
            return "scaling";
            break;
        case Fn::Property::FA_THRESHOLD:
            return "FA threshold";
            break;
        case Fn::Property::EV_THRESHOLD:
            return "EV threshold";
            break;
        case Fn::Property::GAMMA:
            return "gamma";
            break;
        case Fn::Property::OFFSET:
            return "offset";
            break;
        case Fn::Property::RENDER_SLICE:
            return "render slice";
            break;
        case Fn::Property::RENDER_LOWER_X:
            return "render lower x";
            break;
        case Fn::Property::RENDER_UPPER_X:
            return "render upper x";
            break;
        case Fn::Property::RENDER_LOWER_Y:
            return "render lower y";
            break;
        case Fn::Property::RENDER_UPPER_Y:
            return "render upper y";
            break;
        case Fn::Property::RENDER_LOWER_Z:
            return "render lower z";
            break;
        case Fn::Property::RENDER_UPPER_Z:
            return "render upper z";
            break;
        case Fn::Property::MINMAX_SCALING:
            return "minmax scaling";
            break;
        case Fn::Property::BVALUE:
            return "bvalue";
            break;
        case Fn::Property::TENSOR_RENDERMODE:
            return "tensor render moder";
            break;
        case Fn::Property::ISO_VALUE:
            return "iso value";
            break;
        case Fn::Property::GLOBAL_SAGITTAL:
            return "sagittal";
            break;
        case Fn::Property::GLOBAL_CORONAL:
            return "coronal";
            break;
        case Fn::Property::GLOBAL_AXIAL:
            return "axial";
            break;
        case Fn::Property::GLOBAL_MAX_SAGITTAL:
            return "max saggital";
            break;
        case Fn::Property::GLOBAL_MAX_CORONAL:
            return "max coronal";
            break;
        case Fn::Property::GLOBAL_MAX_AXIAL:
            return "max axial";
            break;
        case Fn::Property::GLOBAL_SLICE_DX:
            return "slice dx";
            break;
        case Fn::Property::GLOBAL_SLICE_DY:
            return "slice dy";
            break;
        case Fn::Property::GLOBAL_SLICE_DZ:
            return "slice dz";
            break;
        case Fn::Property::GLOBAL_LAST_PATH:
            return "last path";
            break;
        case Fn::Property::GLOBAL_SHOW_SAGITTAL:
            return "show sagittal";
            break;
        case Fn::Property::GLOBAL_SHOW_CORONAL:
            return "show coronal";
            break;
        case Fn::Property::GLOBAL_SHOW_AXIAL:
            return "show axial";
            break;
        case Fn::Property::GLOBAL_ZOOM:
            return "zoom";
            break;
        case Fn::Property::GLOBAL_MOVEX:
            return "move x";
            break;
        case Fn::Property::GLOBAL_MOVEY:
            return "move y";
            break;
        case Fn::Property::GLOBAL_BBX:
            return "bounding box x";
            break;
        case Fn::Property::GLOBAL_BBY:
            return "bounding box y";
            break;
        case Fn::Property::GLOBAL_VIEW:
            return "view";
            break;
        case Fn::Property::SETTING_LOCK_WIDGETS:
            return "lock widgets";
            break;
        case Fn::Property::FIBER_COLORMODE:
            return "Fiber color mode";
            break;
        case Fn::Property::FIBER_RENDERMODE:
            return "Fiber render mode";
            break;
        default:
            return "no string set for property";
            break;
    }
    return "";
}
