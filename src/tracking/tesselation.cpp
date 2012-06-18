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
#include <boost/smart_ptr.hpp>

#include "tesselation.hpp"
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
const boost::shared_ptr<Matrix> vertices( 

  const unsigned long steps )
{
  switch( steps )
  {
    case(0): return set_vertices( __TESS_ZERO_V,  __TESS_ZERO_V_SIZE );
    
    case(1): return set_vertices( __TESS_ONE_V,   __TESS_ONE_V_SIZE );
    
    case(2): return set_vertices( __TESS_TWO_V,   __TESS_TWO_V_SIZE );
    
    case(3): return set_vertices( __TESS_THREE_V, __TESS_THREE_V_SIZE );
    
    case(4): return set_vertices( __TESS_FOUR_V, __TESS_FOUR_V_SIZE );
    
    case(5): return set_vertices( __TESS_FIVE_V, __TESS_FIVE_V_SIZE );
    
    default: 
        throw std::range_error( "Number of subdivision steps out of range!" );    
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
const boost::shared_ptr<Matrix> faces(

  const unsigned long steps )
{
  switch(steps)
  {
    case(0):return set_faces( __TESS_ZERO_F,  __TESS_ZERO_F_SIZE );

    case(1):return set_faces( __TESS_ONE_F,   __TESS_ONE_F_SIZE );

    case(2):return set_faces( __TESS_TWO_F,   __TESS_TWO_F_SIZE );

    case(3):return set_faces( __TESS_THREE_F, __TESS_THREE_F_SIZE );
    
    case(4):return set_faces( __TESS_FOUR_F, __TESS_FOUR_F_SIZE );
    
    case(5):return set_faces( __TESS_FIVE_F, __TESS_FIVE_F_SIZE );
    
    default:
        throw std::range_error( "Number of subdivision steps out of range!" );
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
const unsigned long n_faces(

  const unsigned long steps )
{
  switch(steps)
  {
    case(0):return __TESS_ZERO_F_SIZE;

    case(1):return __TESS_ONE_F_SIZE;

    case(2):return __TESS_TWO_F_SIZE;

    case(3):return __TESS_THREE_F_SIZE;
    
    case(4):return __TESS_FOUR_F_SIZE;
    
    case(5):return __TESS_FIVE_F_SIZE;
    
    default:
        throw std::range_error( "Number of subdivision steps out of range!" );
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
const unsigned long n_vertices(

  const unsigned long steps )
{
  switch(steps)
  {
    case(0):return __TESS_ZERO_V_SIZE;

    case(1):return __TESS_ONE_V_SIZE;

    case(2):return __TESS_TWO_V_SIZE;

    case(3):return __TESS_THREE_V_SIZE;
    
    case(4):return __TESS_FOUR_V_SIZE;
    
    case(5):return __TESS_FIVE_V_SIZE;
    
    default:
        throw std::range_error( "Number of subdivision steps out of range!" );
  }
}
//******************************************************************************

///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const Vector get_vertex(
///* 
///*     const unsigned long position,
///*     const unsigned long steps )
///* 
///* @brief Returns a single vertex at a certain position for a given number 
///*        of tesselation steps.
///* 
///* @param Scalar: p position of vertex.
///* @param Scalar: n number of tesselation subdivisions.
///* 
///* @return Vector: The vertex at position.
///*****************************************************************************
const Vector get_vertex(

  const unsigned long position,
  const unsigned long steps )
{
  unsigned long size( n_vertices( steps ) );
  if( position < size )
  {
    Vector result(3);
    switch(steps)
    {
      case(0):
      {
        result(0) = __TESS_ZERO_V[ 3 * position ];
        result(1) = __TESS_ZERO_V[ 3 * position + 1 ];
        result(2) = __TESS_ZERO_V[ 3 * position + 2 ];
        return result;
      }

      case(1):
      {
        result(0) = __TESS_ONE_V[ 3 * position ];
        result(1) = __TESS_ONE_V[ 3 * position + 1 ];
        result(2) = __TESS_ONE_V[ 3 * position + 2 ];
        return result;
      }

      case(2):
      {
        result(0) = __TESS_TWO_V[ 3 * position ];
        result(1) = __TESS_TWO_V[ 3 * position + 1 ];
        result(2) = __TESS_TWO_V[ 3 * position + 2 ];
        return result;
      }

      case(3):
      {
        result(0) = __TESS_THREE_V[ 3 * position ];
        result(1) = __TESS_THREE_V[ 3 * position + 1 ];
        result(2) = __TESS_THREE_V[ 3 * position + 2 ];
        return result;
      }
      
      case(4):
      {
        result(0) = __TESS_FOUR_V[ 3 * position ];
        result(1) = __TESS_FOUR_V[ 3 * position + 1 ];
        result(2) = __TESS_FOUR_V[ 3 * position + 2 ];
        return result;
      }
      
      case(5):
      {
        result(0) = __TESS_FIVE_V[ 3 * position ];
        result(1) = __TESS_FIVE_V[ 3 * position + 1 ];
        result(2) = __TESS_FIVE_V[ 3 * position + 2 ];
        return result;
      }
      
      default:
        throw std::range_error( "Number of subdivision steps out of range!" );
    }
  }
  else
    throw std::range_error( "Requested vertex out of range!" );
}
//******************************************************************************

///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const size_Vector get_face(
///* 
///*     const unsigned long position,
///*     const unsigned long steps )
///* 
///* @brief Returns single face at position for given number of tesselation steps.
///* 
///* @param Scalar: p position of face.
///* @param Scalar: n number of tesselation subdivisions.
///* 
///* @return Vector: The face at position.
///*****************************************************************************
const Vector get_face(

  const unsigned long position,
  const unsigned long steps )
{
  unsigned long size( n_faces( steps ) );
  if( position < size )
  {
    Vector result(3);
    switch(steps)
    {
      case(0):
      {
        result(0) = __TESS_ZERO_F[ 3 * position ];
        result(1) = __TESS_ZERO_F[ 3 * position + 1 ];
        result(2) = __TESS_ZERO_F[ 3 * position + 2 ];
        return result;
      }
  
      case(1):
      {
        result(0) = __TESS_ONE_F[ 3 * position ];
        result(1) = __TESS_ONE_F[ 3 * position + 1 ];
        result(2) = __TESS_ONE_F[ 3 * position + 2 ];
        return result;
      }
  
      case(2):
      {
        result(0) = __TESS_TWO_F[ 3 * position ];
        result(1) = __TESS_TWO_F[ 3 * position + 1 ];
        result(2) = __TESS_TWO_F[ 3 * position + 2 ];
        return result;
      }
  
      case(3):
      {
        result(0) = __TESS_THREE_F[ 3 * position ];
        result(1) = __TESS_THREE_F[ 3 * position + 1 ];
        result(2) = __TESS_THREE_F[ 3 * position + 2 ];
        return result;
      }
      
      case(4):
      {
        result(0) = __TESS_FOUR_F[ 3 * position ];
        result(1) = __TESS_FOUR_F[ 3 * position + 1 ];
        result(2) = __TESS_FOUR_F[ 3 * position + 2 ];
        return result;
      }
      
      case(5):
      {
        result(0) = __TESS_FIVE_F[ 3 * position ];
        result(1) = __TESS_FIVE_F[ 3 * position + 1 ];
        result(2) = __TESS_FIVE_F[ 3 * position + 2 ];
        return result;
      }
      
      default:
          throw std::range_error( "Number of subdivision steps out of range!" );
    }
  }
  else
      throw std::range_error( "Requested face out of range!" );
}
//******************************************************************************

///*****************************************************************************
///* @file Tesselation.cpp
///* @fn const Matrix adjacency(
///* 
///*     const unsigned long steps )
///* 
///* @brief Creates an adjacency matrix from the tesselation information. 
///* 
///* Does this by traversing the matrix of faces and builds a matrix in which for 
///* the index of each row the corresponding row is a vector of adjacent indices.
///* 
///* @param Scalar: Number of tesselation steps.
///* 
///* @return Matrix: Indices adjacent to the index (row) of the matrix.
///*****************************************************************************
const boost::shared_ptr<Matrix> adjacency(
 
  const unsigned long steps )
{
  // allocate memory for output object and initialize with zeros.
  boost::shared_ptr<Matrix> result( new Matrix( n_vertices( steps ), 6, true ) );
  
  // go through all faces for retrieving neighborhood information.
  for( unsigned long i(0); i < n_faces( steps ); i++ )
  {
    Vector face( get_face( i, steps ) );

    // for all components of face vector.
    for( unsigned long j(0); j < 3; j++ )
    {
      // marker if one of the two other points have to be added:
      bool first( true ), second( true );
      
      // check which one hast to be added:
      unsigned long index(0);
      for( index = 0; 
           index < 6 && (*result)( face(j), index ) && (first||second); 
           index++ )
      {
        // found first:
        if( first ) 
          if( (*result)( face(j), index ) == face( (j+1)%3 ) + 1 ) 
          first = false;
        if( second ) 
          if( (*result)( face(j), index ) == face( (j+2)%3 ) + 1 ) 
          second = false;
      }
      if( first && second ) 
      {
        (*result)( face(j), index ) = face( (j+1)%3 ) + 1;
        (*result)( face(j), index + 1 ) = face( (j+2)%3 ) + 1;
      }
      else if ( first ) (*result)( face(j), index ) = face( (j+1)%3 ) + 1;
      else if ( second ) (*result)( face(j), index ) = face( (j+2)%3 ) + 1;
    }
  }
  // subtract one from all elements in result.
  for( unsigned long i(0); i < (*result).size1(); i++ )
  {
    for( unsigned long j(0); j < (*result).size2(); j++ )
    {
      (*result)(i,j)--;
    }
  }
  return result;
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
const boost::shared_ptr<Matrix> set_vertices(

  const double input[],
  const unsigned long size )
{
  
  // allocate output object.
  boost::shared_ptr<Matrix> result( new Matrix( size, 3 ) );
  
  for( unsigned long i(0); i < size; i++ )
  {
    (*result)( i, 0 ) = input[ 3 * i + 0 ];
    (*result)( i, 1 ) = input[ 3 * i + 1 ];
    (*result)( i, 2 ) = input[ 3 * i + 2 ];
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
const boost::shared_ptr<Matrix> set_faces(

  const unsigned long input[],
  const unsigned long size )
{
  // allocate output object.
  boost::shared_ptr<Matrix> result( new Matrix( size, 3 ) );

  for( unsigned long i(0); i < size; i++ )
  {
    (*result)( i, 0 ) = input[ 3 * i + 0 ];
    (*result)( i, 1 ) = input[ 3 * i + 1 ];
    (*result)( i, 2 ) = input[ 3 * i + 2 ];
  }

  // return result.
  return result;
}
//******************************************************************************
} // End of namespace tess.
