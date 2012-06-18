//* ****************************************************************************
//*                                                                            
//* Copyright (C) 2010 by Till W. Riffert             
//*                                                                            
//* Max Planck Institute for Human Cognitive and Brain Sciences.                 
//* PO Box 500 355, D-04303 Leipzig, Germany                                   
//*                                                                                                                                                                                                                                                            
//* Email riffert@cbs.mpg.de             
//* WWW   www.cbs.mpg.de                                                       
//*                                                                                                                                                       
//* ****************************************************************************

#ifndef INC_IMAGE_STRUCT_H
#define INC_IMAGE_STRUCT_H
 
#include "../data/nifti/nifti1.h"
#include "../data/nifti/nifti1_io.h"

#include <boost/smart_ptr.hpp>

#include "math_structs.hpp"

#include <memory>
#include <cstdlib>
#include <vector>

#include <omp.h>

#define BOOST_SP_USE_QUICK_ALLOCATOR
//* ****************************************************************************
//* ENUMS
//* ****************************************************************************

///* ***************************************************************************
///* 
///* @file image_struct.h
///* 
///* @enum nifti_type
///* @brief Is used to select the datatype (see nifti1.h)
///* with SAME you could select the same datatype for output like input
///* 
///* ***************************************************************************
enum nifti_type
{ 
  CHAR   = 2,
  SHORT  = 4,
  INT    = 8,
  FLOAT  = 16,
  DOUBLE = 64
};
//* ****************************************************************************

//* ****************************************************************************
//* CLASS
//* ****************************************************************************

class Image
{
private:
  nifti_1_header header_;
  unsigned long x_, y_, z_;
  std::vector< boost::shared_ptr<Vector> > data_;

protected:
  template< typename TYPE >
  void read_nii(const nifti_image&, std::vector<boost::shared_ptr<Vector> >&);
  template < typename TYPE > 
  void write_nii(const char* file); 
  
public:
  nifti_1_header get_header() const { return header_; };
  const unsigned long get_x() const { return x_; };
  const unsigned long get_y() const { return y_; };
  const unsigned long get_z() const { return z_; };
  
  const Matrix get_sform() const;
  
  void write( const char* file, nifti_type type=DOUBLE );
  
  Image();
  
  Image( const Image& input );
  
  Image( const char* file );
  
  Image( const nifti_1_header header, 
          const unsigned long x, 
          const unsigned long y, 
          const unsigned long z );
  
  Image( const nifti_1_header header, 
          const unsigned long x, 
          const unsigned long y, 
          const unsigned long z, 
          const std::vector< boost::shared_ptr<Vector> >& data );
  
  // operators:
  boost::shared_ptr<Vector> & operator() 
      ( const unsigned long x, 
        const unsigned long y,
        const unsigned long z );

  const boost::shared_ptr<Vector> & get(const unsigned long x, 
        const unsigned long y,
        const unsigned long z ) const;
  
  void extract_b_zero( const Vector& g_mask, Image& data, Image& b_zero );
};

//* ****************************************************************************

               
//* ****************************************************************************
// INLINE FUNCTIONS:               
//* ****************************************************************************
               
inline boost::shared_ptr<Vector> & Image::operator() 
  ( const unsigned long pos1, 
    const unsigned long pos2,
    const unsigned long pos3 )
{
  return data_[pos1 + pos2 * x_ + pos3 * x_ * y_ ];
}          

inline const boost::shared_ptr<Vector> & Image::get
  ( const unsigned long pos1, 
    const unsigned long pos2,
    const unsigned long pos3 ) const
{
  return data_[pos1 + pos2 * x_ + pos3 * x_ * y_ ];
}

inline const Matrix Image::get_sform() const
{
  Matrix result(3,3);
  result(0,0) = header_.srow_x[0]; 
  result(0,1) = header_.srow_x[1]; 
  result(0,2) = header_.srow_x[2];
    
  result(1,0) = header_.srow_y[0]; 
  result(1,1) = header_.srow_y[1]; 
  result(1,2) = header_.srow_y[2];
  
  result(2,0) = header_.srow_z[0]; 
  result(2,1) = header_.srow_z[1]; 
  result(2,2) = header_.srow_z[2];
  
  return result;
}
#endif // INC_IMAGE_STRUCT_H

