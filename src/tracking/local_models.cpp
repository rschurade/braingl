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
 
#include "local_models.hpp"
#include "math.hpp"
 
#include <boost/math/special_functions/legendre.hpp>
 
///* ***************************************************************************
///* @file Tensor.cpp
///* @fn Matrix direction_matrix(
///* 
///*     const Matrix& g )
///* 
///* @brief Build the direction matrix from the gradient matrix
///* 
///* @param gradients
///* 
///* @return direction matrix: xx xy xz yy yz zz for each gradient
///* ***************************************************************************
boost::shared_ptr<Matrix> direction_matrix(

  const Matrix& g )
{
  if( g.size1() == 0 ) 
    throw std::invalid_argument( "No gradient directions specified." );
    
  if( g.size2() != 3 )
    throw std::invalid_argument( "Gradients have to be 3D." );
    
  // allocate direction matrix:
  boost::shared_ptr<Matrix> d_matrix( new Matrix( g.size1(), 6 ) );
  for( unsigned long i(0); i < g.size1(); i++ )
  {               
    (*d_matrix)(i,0) =      g(i,0) * g(i,0);
    (*d_matrix)(i,1) = 2. * g(i,0) * g(i,1);
    (*d_matrix)(i,2) = 2. * g(i,0) * g(i,2);
    (*d_matrix)(i,3) =      g(i,1) * g(i,1);
    (*d_matrix)(i,4) = 2. * g(i,1) * g(i,2);
    (*d_matrix)(i,5) =      g(i,2) * g(i,2);
  }

  // resize to correct dimensionality and return:
  return d_matrix;
}
//* ****************************************************************************



///* ***************************************************************************
///* @file Tensor.cpp
///* @fn double fa(
///* 
///*     const Vector &eigenvalues )
///* 
///* @brief Calculates the fractional anisotropy
///* 
///* FA = sqrt((e0-e1)^2 + (e0-e2)^2 + (e1-e2)^2 )/sqrt( 2.*( e0^2+e1^2+e2^2 ))
///* 
///* @param Eigenvalues sorted in descending order
///* 
///* @return Fractional anisotropy (FA)
///* ***************************************************************************
double fa(

  const Vector &ev )
{
  if( ev.size() != 3 )
    throw std::invalid_argument("Exactly 3 eigenvalues required!");
        
  double a( (fabs( ev(0) ) - fabs( ev(1) )) * (fabs( ev(0) ) - fabs( ev(1) ))
          + (fabs( ev(0) ) - fabs( ev(2) )) * (fabs( ev(0) ) - fabs( ev(2) ))
          + (fabs( ev(1) ) - fabs( ev(2) )) * (fabs( ev(1) ) - fabs( ev(2) ))),
          b( ev(0)*ev(0) + ev(1)*ev(1) + ev(2)*ev(2) );
          
  // if denominator is zero return zero       
  if( b <= 0.0 ) 
    return 0.0;
  else 
    return sqrt( a / ( 2. * b ) );
}
//* ****************************************************************************

///* ***************************************************************************
///* @file Tensor.cpp
///* @fn double fa(
///* 
///*     const Vector &eigenvalues )
///* 
///* @brief Calculates the fractional anisotropy
///* 
///* FA = sqrt((e0-e1)^2 + (e0-e2)^2 + (e1-e2)^2 )/sqrt( 2.*( e0^2+e1^2+e2^2 ))
///* 
///* @param Eigenvalues sorted in descending order
///* 
///* @return Fractional anisotropy (FA)
///* ***************************************************************************
double adc(

  const Vector &S,
  const double &S0,
  const double b )
{
  double result(0);
  for( unsigned int i(0); i < S.size(); i++ )
  {
    if( S(i) > 0 && S0 > 0 )
      result += -b * log(S(i)/S0);
  }
  return result/static_cast<double>(S.size());
}
//* ****************************************************************************


///* ***************************************************************************
///* @file Tensor.cpp
///* @fn Vector tensor(
///* 
///*     const Vector& data, 
///*     const Matrix& inverse_direction_matrix, 
///*     const double b_zero_signal, 
///*     const double b_value )
///* 
///* @brief Calculates the diffusion tensor from a set of gradients 
///*        and diffusion data
///* 
///*  For further information view:                                         <br>
///*  Mori, Introduction to Diffusion Tensor Imaging, Elsevier 2007
///* 
///* @param data: vector of the diffusion measurements within a single voxel
///* @param gradients: matrix of the diff gradient directions of form (x,y,z)
///* @param b_zero_signal: the measured b-zero value within the voxel
///* @param b_value: the b-value for which the measurement was performed 
///* 
///* @return A vector containng the diffusion tensor information ordered as
///*                         xx, xy, xz, yy, yz, zz
///* ***************************************************************************
boost::shared_ptr<Vector> tensor(

  const Vector& data, 
  const Matrix& inv_dir_matrix, 
  const double b_zero_signal, 
  const double b_value )
{
  // if empty return zero:
  if( data.norm() <= 0.0 || b_zero_signal <= 0.0 ) 
    return boost::shared_ptr<Vector>( new Vector(6,true) );

  // apparent diffusion in respect to gradient directions:
  Vector diff( (*(log( data / b_zero_signal ))) * ( -(1.0/b_value) ) );
  
  // multiply diffusion data and direction matrix:
  return boost::shared_ptr<Vector>( new Vector( prod( inv_dir_matrix, diff ) ) );
}
//* ****************************************************************************



///* ***************************************************************************
///* @file Q_Ball.cpp
///* @fn matrixT q_ball(
///* 
///*     const matrixT& gradients,
///*     const double lambda,
///*     const intT M )
///* 
///* @brief Function for computing the q-ball model base.
///* 
///* 1) Get sh base B,                                                      <br>
///* 2) Calculate Laplace Beltrami operator L,                              <br>
///* 3) Calculate Funk Radon transform P,                                   <br>
///* 4) T = P*inv(B'B+lambda*L)*B'                                          <br>
///* 
///* For further information view:                                          <br>
///*  Descoteaux, et al., Magnetic Resonance in Medicine (2007) 
///* 
///* 
///* @param Matrix: gradients,
///* @param Scalar: influence of Laplace Beltrami operator,
///* @param Scalar: maximum order of spherical harmonic approximation.
///* 
///* @return Matrix: The q-ball transformation matrix.
///* ***************************************************************************
boost::shared_ptr<Matrix> q_ball(

  const Matrix& gradients,
  const double lambda,
  const int max_order )
{
  double sh_size( (max_order+1) * (max_order+2) / 2 );
  
  // check validity of input:
  if( gradients.size1() == 0 ) 
      throw std::invalid_argument( "No gradient directions specified." );
    
  if( gradients.size2() != 3 )
      throw std::invalid_argument( "Gradients have to be 3D." );
   
  // calculate spherical harmonics base:
  const boost::shared_ptr<Matrix> SH( sh_base( gradients, max_order ) );

  // calculate the Laplace-Beltrami and the Funk-Radon transformation:
  Vector LBT( sh_size ), FRT( sh_size );

  for( int order(0); order <= max_order; order+=2 )
  {
    double frt_val = 2.0 * M_PI * boost::math::legendre_p<double>( order, 0 );
    double lbt_val = lambda * order * order * ( order + 1 ) * ( order + 1 );
    
    for( int degree( -order ); degree <= order; degree++ )
    {
      int i =  order * (order+1) / 2 + degree;
      LBT(i) = lbt_val; 
      FRT(i) = frt_val;
    }
  }
    // prepare the calculation of the pseudoinverse: 
    Matrix B( prod( trans( *SH ), *SH ) );
    
    // update with Laplace-Beltrami operator:
    for( int i(0); i < sh_size; i++ ) B(i,i) += LBT(i); 

    boost::shared_ptr<Matrix> out( new Matrix(prod((*inverse(B)),trans((*SH)))) );
    // the Funk-Radon transformation:
    for( unsigned long i(0); i < B.size1(); i++ )
        for( unsigned long j(0); j < B.size2(); j++ ) (*out)(i,j) *= FRT(i);

    // done:
    return out;    
}
//* ****************************************************************************

///* ***************************************************************************
///* @file Tensor.cpp
///* @fn TODO
///* 
///* @brief TODO
///* @param TODO
///* 
///* @return TODO
///* ***************************************************************************
Vector kernel(
  const Vector& data,
  const Vector& b_zero,
  const Matrix& inv_dir_matrix,
  const Matrix& gradients,
  const int order,
  const double b_value )
{  
  // Allocate temporal objects:
  Vector response( order/2 + 1, true );
  std::vector<Vector> vec; 
  Vector val(3);

  // Calculate tensorfit:
  boost::shared_ptr<Vector> dwi( new Vector(*log(data/b_zero(0))*(-1.0/b_value)));
  boost::shared_ptr<Vector> dt( new Vector(prod(inv_dir_matrix, *dwi) ));

  // Eigenvalue decomposition of the diffusion tensor:
  evd3x3( *dt, val, vec );

  // If eigenvalues exist:
  if( val.norm() > 0.0 )
  {
    // Rotate eigenvectors:
    Matrix vec_asc(3,3);
    vec_asc(0,0)=vec[2](0); vec_asc(0,1)=vec[2](1); vec_asc(0,2)=vec[2](2);
    vec_asc(1,0)=vec[1](0); vec_asc(1,1)=vec[1](1); vec_asc(1,2)=vec[1](2);
    vec_asc(2,0)=vec[0](0); vec_asc(2,1)=vec[0](1); vec_asc(2,2)=vec[0](2);
    
    // Create rotated gradients:
    Matrix rot_grad( gradients.size1(), 3 );
    for( unsigned long i(0); i < gradients.size1(); i++ )
    {
      // Current gradient:
      Vector cur(3);
      cur(0) = gradients(i,0);
      cur(1) = gradients(i,1);
      cur(2) = gradients(i,2);
      
      // Rotate gradient using rotated eigenvectors:
      Vector tmp( prod( vec_asc, cur ) );
      rot_grad(i,0) = tmp(0);
      rot_grad(i,1) = tmp(1);
      rot_grad(i,2) = tmp(2);   
    }
  
    // Calculate spherical harmonic representation of dirac delta:
    const Matrix B( *pseudoinverse( *sh_base( rot_grad, order ) ) );
    const Vector A( prod( B, data ) );
    
    // Update response:
    for( int i(0); i < order/2 + 1; i++ ) response(i) += A(2*i*i +i); 
  }              
  return response;
}

/* ****************************************************************************/
