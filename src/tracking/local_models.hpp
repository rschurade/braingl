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

#ifndef INC_LOCAL_MODELS_H
#define INC_LOCAL_MODELS_H

// defines:
#define _USE_MATH_DEFINES

// includes:
#include <boost/smart_ptr.hpp>

#include "math_structs.hpp"

#include <memory>
#include <cstdlib>
#include <vector>

boost::shared_ptr<Matrix> direction_matrix(
  const Matrix& g );

double fa(
  const Vector &ev );

double adc(
  const Vector &S,
  const double &S0,
  const double b );

boost::shared_ptr<Vector> tensor(
  const Vector& data, 
  const Matrix& inv_dir_matrix, 
  const double b_zero_signal, 
  const double b_value );

boost::shared_ptr<Matrix> q_ball(
  const Matrix& gradients,
  const double lambda,
  const int max_order );

  // deconvolution.cpp
boost::shared_ptr<Vector> spherical_deconvolution(
  const Vector& voxel_data,
  const Matrix& inv_sd_matrix  );

boost::shared_ptr<Matrix> inv_sd_rot(
  const Matrix& gradients,
  const std::vector<double>& kernel );

Vector kernel(
  const Vector& data,
  const Vector& b_zero,
  const Matrix& inv_dir_matrix,
  const Matrix& gradients,
  const int order,
  const double b_value );

#endif // INC_LOCAL_MODELS_H
