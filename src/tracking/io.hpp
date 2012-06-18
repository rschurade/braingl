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

#include "math_structs.hpp"
#include "image_structs.hpp"

Matrix read_gradients( 
  const char *file_bval, 
  const char *file_bvec, 
  Vector& b_values );

std::vector<double> read_kernel( const char *file );

std::vector<Vector> read_voxels( const char *file );

Matrix orient_gradients( const Matrix& gradients, const Matrix& sform );

void write_kernel( const std::vector<double>& output, const char *filename );

void write_sh_glyph( const char *filename, const Image&,
                     const unsigned int, const double );

void write_bingham_glyph(

  const char *filename,
  const Image& f0,
  const Image& m1,
  const Image& m2,
  const Image& k1,
  const Image& k2,
  const unsigned int steps,
  const double color );

double mri_color( const double X, const double Y, const double Z );
