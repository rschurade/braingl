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
//* ***************************************************************************/

#ifndef INC_MATH_H
#define INC_MATH_H

// defines:
#define _USE_MATH_DEFINES

#include <boost/smart_ptr.hpp>

// includes:
#include "math_structs.hpp"

#include <memory>
#include <cstdlib>
#include <vector>

// std_math.cpp

void bubble_sort(
  Vector& values,
  std::vector<int>& );

boost::shared_ptr<Vector> cart2sphere( 
  const Vector& );

boost::shared_ptr<Matrix> inverse(
  const Matrix& );

boost::shared_ptr<Vector> log(
  const Vector& );

boost::shared_ptr<Matrix> pseudoinverse(
  const Matrix& );

boost::shared_ptr<Vector> sphere2cart(
  const Vector& );

boost::shared_ptr<Vector> QR_LS_solve( 
  const Matrix&,    
  const Vector& );

boost::shared_ptr<Vector> cross3x3( 
  const Vector&, 
  const Vector& );

boost::shared_ptr<Vector> cross3x3( 
  const Vector&, 
  const Vector& );

void evd3x3(
  const Vector &d, 
  Vector &val, 
  std::vector<Vector> &vec );

boost::shared_ptr<Matrix> sh_base(
    const Matrix&,
    const int );

double sh_base_function(
  const int,
  const int,
  const double,
  const double );

double sh_eval(
  const Vector&,
  const Vector&);

bool is_local_min(
  const Vector& values,
  const Matrix& adjacency,
  unsigned long i );

boost::shared_ptr<Vector> ranked_maxima(
  const Vector& radius,
  const Matrix& tesselation,
  const Matrix& adjacency,
  const unsigned long num_max );

boost::shared_ptr< std::vector<unsigned long> > ranked_maxima_index(
  const Vector& radius,
  const Matrix& adjacency,
  const unsigned long num_max );

boost::shared_ptr< std::vector<unsigned long> >local_maxima(    
  const Matrix& adjacent,
  const Vector& values );

boost::shared_ptr<Vector> fit_bingham(
  const Vector& sh_data,
  const Matrix& tesselation,
  const Matrix& adj,
  const Matrix& base,
  const unsigned long neighbourhood,
  const unsigned long num_max );

#endif // INC_MATH_H
