//******************************************************************************
//*                                                                            
//* Copyright (C) 2010 by Till W. Riffert             
//*                                                                            
//* Max Planck Institute for Human Cognitive and Brain Sciences.                 
//* PO Box 500 355, D-04303 Leipzig, Germany                                   
//*                                                                                                                                                                                                                                                            
//* Email riffert@cbs.mpg.de             
//* WWW   www.cbs.mpg.de                                                       
//*                                                                                                                                                       
//*****************************************************************************/
#include "tesselation.h"
namespace tess
{
//******************************************************************************
//* DEFINES CONSTANT TESSELATIONS                                    
//******************************************************************************
//* 
//* 4 Tesselation degrees are defined:
//* 
//*  __TESS_ZERO:   12 vertices    20 faces
//*  __TESS_ONE:    42 vertices    80 faces
//*  __TESS_TWO:   162 vertices   320 faces
//*  __TESS_THREE: 642 vertices  1280 faces
//*  __TESS_FOUR: 2,562 vertices  5,120 faces
//*  __TESS_FIVE: 10,242 vertices 20,480 faces
//* 
//******************************************************************************

///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const Matrix vertices( 
///* 
///*     const unsigned long steps )
///* 
///* @brief Accesses lookuptable for vertices of tesselation.
///* 
///* @param Scalar: n number of tesselation subdivisions.
///* 
///* @return Matrix: vertices of the tesselation with n subdivison steps.
///*****************************************************************************
const Matrix* vertices( const unsigned long steps )
{
    switch ( steps )
    {
        case ( 0 ):
            return set_vertices( __TESS_ZERO_V, __TESS_ZERO_V_SIZE );

        case ( 1 ):
            return set_vertices( __TESS_ONE_V, __TESS_ONE_V_SIZE );

        case ( 2 ):
            return set_vertices( __TESS_TWO_V, __TESS_TWO_V_SIZE );

        case ( 3 ):
            return set_vertices( __TESS_THREE_V, __TESS_THREE_V_SIZE );

        case ( 4 ):
            return set_vertices( __TESS_FOUR_V, __TESS_FOUR_V_SIZE );

        case ( 5 ):
            return set_vertices( __TESS_FIVE_V, __TESS_FIVE_V_SIZE );

        default:
            //throw std::range_error( "Number of subdivision steps out of range!" );
            break;
    }
}
//******************************************************************************



///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const Matrix faces(
///* 
///*     const unsigned long steps )
///* 
///* @brief Accesses lookuptable for vertices of tesselation.
///* 
///* @param Scalar: n number of tesselation subdivisions.
///* 
///* @return Matrix: faces of the tesselation with n subdivison steps.
///*****************************************************************************
const int* faces( const unsigned long steps )
{
    switch ( steps )
    {
        case ( 0 ):
            return __TESS_ZERO_F; //set_faces( __TESS_ZERO_F, __TESS_ZERO_F_SIZE );

        case ( 1 ):
            return __TESS_ONE_F; //set_faces( __TESS_ONE_F, __TESS_ONE_F_SIZE );

        case ( 2 ):
            return __TESS_TWO_F; //set_faces( __TESS_TWO_F, __TESS_TWO_F_SIZE );

        case ( 3 ):
            return __TESS_THREE_F; //set_faces( __TESS_THREE_F, __TESS_THREE_F_SIZE );

        case ( 4 ):
            return __TESS_FOUR_F; //set_faces( __TESS_FOUR_F, __TESS_FOUR_F_SIZE );

        case ( 5 ):
            return __TESS_FIVE_F; //set_faces( __TESS_FIVE_F, __TESS_FIVE_F_SIZE );

        default:
            //throw std::range_error( "Number of subdivision steps out of range!" );
            break;
    }
}
//******************************************************************************



///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const unsigned long n_faces(
///*     const unsigned long steps )
///* 
///* @brief Returns the number of faces for a given number of tesselation steps.
///* 
///* @param Scalar: n number of tesselation subdivisions.
///* 
///* @return Scalar: number of faces.
///*****************************************************************************
const unsigned long n_faces( const unsigned long steps )
{
    switch ( steps )
    {
        case ( 0 ):
            return __TESS_ZERO_F_SIZE;

        case ( 1 ):
            return __TESS_ONE_F_SIZE;

        case ( 2 ):
            return __TESS_TWO_F_SIZE;

        case ( 3 ):
            return __TESS_THREE_F_SIZE;

        case ( 4 ):
            return __TESS_FOUR_F_SIZE;

        case ( 5 ):
            return __TESS_FIVE_F_SIZE;

        default:
            //throw std::range_error( "Number of subdivision steps out of range!" );
            break;
    }
}
//******************************************************************************



///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const unsigned long n_vertices(
///* 
///*     const unsigned long steps )
///* 
///* @brief Returns the number of vertices for given number of tesselation steps
///* 
///* @param Scalar: n number of tesselation subdivisions.
///* 
///* @return Scalar: number of vertices.
///*****************************************************************************
const unsigned long n_vertices( const unsigned long steps )
{
    switch ( steps )
    {
        case ( 0 ):
            return __TESS_ZERO_V_SIZE;

        case ( 1 ):
            return __TESS_ONE_V_SIZE;

        case ( 2 ):
            return __TESS_TWO_V_SIZE;

        case ( 3 ):
            return __TESS_THREE_V_SIZE;

        case ( 4 ):
            return __TESS_FOUR_V_SIZE;

        case ( 5 ):
            return __TESS_FIVE_V_SIZE;

        default:
           //throw std::range_error( "Number of subdivision steps out of range!" );
            break;
    }
}

//******************************************************************************

///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const Matrix set_vertices(
///* 
///*     const T input[], 
///*     const unsigned long size )
///* 
///* @brief Copies the vertices from a std::array to a Matrix object.
///* 
///* 
///* @param Array: tesselation data.
///* @param Scalar: size.
///* 
///* @return Matrix: including tesselation data.
///*****************************************************************************
const Matrix* set_vertices( const double input[], const unsigned long size )
{
    // allocate output object.
    Matrix* result( new Matrix( size, 3 ) );

    for ( unsigned long i( 0 ); i < size; i++ )
    {
        (*result)( i, 1 ) = input[ 3 * i + 0 ];
        (*result)( i, 2 ) = input[ 3 * i + 1 ];
        (*result)( i, 3 ) = input[ 3 * i + 2 ];
    }

    // return result.
    return result;
}
//******************************************************************************

///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const Matrix set_faces(
///*
///*     const T input[],
///*     const unsigned long size )
///*
///* @brief Copies the faces from a std::array to a Matrix object.
///*
///*
///* @param Array: tesselation data.
///* @param Scalar: size.
///*
///* @return Matrix: including tesselation data.
///*****************************************************************************
const Matrix* set_faces( const unsigned long input[], const unsigned long size )
{
    // allocate output object.
    Matrix* result( new Matrix( size, 3 ) );

    for ( unsigned long i( 0 ); i < size; i++ )
    {
        (*result)( i + 1, 1 ) = input[3 * i + 0];
        (*result)( i + 1, 2 ) = input[3 * i + 1];
        (*result)( i + 1, 3 ) = input[3 * i + 2];
    }

    // return result.
    return result;
}
//******************************************************************************
} // End of namespace tess.
