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

#include "math.hpp"
#include <limits>
#include <boost/math/special_functions/spherical_harmonic.hpp>

///* ***************************************************************************
///* @file math.cpp
///* @fn void bubble_sort(
///* 
///*       Vector& values,
///*       std::vector<int>& indices )
///* 
///* @brief Sorts indices and entries in vector using the bubblesort algorithm.
///* 
///* important: this function works in place.                               <br>
///* 
///* For further information view:                                          <br>
///*  Cormen, Leiversn, Rivest, and Stein: Introduction to Algorithms       <br>
///*  Wikipedia article on bubble sort.
///* 
///*
///* @param Vector: values the vector which determies the sorting
///* @param Vector: indices to be sorted going along 
///* ***************************************************************************
void bubble_sort(

  Vector& values,
  std::vector<unsigned long>& indices )
{ 
  bool flag( true );
  for ( unsigned long j( indices.size() ); j > 0 && flag; j-- )
  {
    flag = false;
    for ( unsigned long i( 1 ); i < indices.size(); i++ )
    {
      if ( values( i - 1 ) < values( i ) )
      {
        double tmp_val = values( i - 1 );
        values( i - 1 ) = values( i );
        values( i ) = tmp_val;
        
        unsigned long tmp_index = indices[ i - 1 ];
        indices[ i - 1 ] = indices[ i ];
        indices[ i ] = tmp_index;
        flag = true;
      }
    } 
  }
}
//******************************************************************************


///* ***************************************************************************
///* @file math.cpp
///* @fn Vector cart2sphere( 
///* 
///*     const Vector& input )
///* 
///* @brief Transforms a cartesian-coordinate vector to spherical coordinates.
///* 
///* For further information view:                                          <br>
///*  Wikipedia article on cartesian and spherical coordinates.
///* 
///* @param Vector: a in cartesian-coordinates 
///* 
///* @return Vector: representation of a in spherical coordinates, 
///*                 order: r, theta, phi.
///* ***************************************************************************
boost::shared_ptr<Vector> cart2sphere( 

  const Vector& input )
{
  if ( input.size() != 3 ) 
    throw std::range_error("Can only transform 3D-vectors.");
        
  boost::shared_ptr<Vector> result( new Vector(3, true) );

  (*result)(0) =  sqrt( input(0) * input(0) + 
                        input(1) * input(1) + 
                        input(2) * input(2) );
  (*result)(1) =  acos( input(2) / (*result)(0) );
  (*result)(2) = atan2( input(1),  input(0) );

  return result;
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn Matrix inverse(
///*     const Matrix& input )
///* 
///* @brief Function for computing the inverse of a matrix.
///* 
///* Uses GSL Library                                                       <br>
///* This matrix is calculated using the LU factorisation of A.             <br>
///* 
///* For further information view:                                          <br>
///*  Any book on linear algebra or basic computational methods / numerics.
///* 
///* @param Matrix: A
///* 
///* @return Matrix: INV which satisfies  A * INV = Identity.
///* ***************************************************************************
boost::shared_ptr<Matrix> inverse(

  const Matrix& input )
{

  if ( input.size1() != input.size2() )
    throw std::range_error( "Matrix dimensions do not match!" );

  // allocate temporal gsl objects:
  gsl_permutation *p = gsl_permutation_alloc( input.size1() );
  gsl_matrix *result= gsl_matrix_alloc( input.size1(), input.size2() );


  // LU decomposition and solution of linear system:
  int signum;
  gsl_linalg_LU_decomp( input.get_gsl(), p, &signum );
  gsl_linalg_LU_invert( input.get_gsl(), p, result );

  // free temporal objects and return result:
  gsl_permutation_free( p );

  return boost::shared_ptr<Matrix>( new Matrix( result ) );
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn Vector log(
///*     const Vector& in )
///* 
///* @brief Computes the elementwise logarithm of a vector.
///* 
///* This logarithm function returns -1.7e-17 if logarithm of number <= 0.0
///* 
///* 
///* @param Vector: v
///* 
///* 
///* @return Vector: log(v) 
///* ***************************************************************************
boost::shared_ptr<Vector> log(

  const Vector& input )
{
  // allocate memory of output object:
  boost::shared_ptr<Vector> tmp( new Vector( input.size() ) );

  // for each element of the vector:
  for ( unsigned long i(0); i < input.size(); i++ )
    if ( input(i) <= 0.0  ) (*tmp)(i) = -1.0e-17;
    else (*tmp)(i) = log( input(i) );

  return tmp;
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn Matrix pseudoinverse(
///* 
///* 	const Matrix& input )
///* 
///* @brief Function for computing the (Moore-Penrose-)pseudoinverse of a matrix.
///* 
///* This matrix is calculated using INV=inv(A'*A)*A'.                      <br>
///* Uses the GSL Library                                                   <br>
///* For further information view:                                          <br>
///*  Roger Penrose: A generalized inverse for matrices. 1955.
///* 
///* @param Matrix: A
///* 
///* @return Matrix: INV which satisfies  A*INV*A = INV && INV*A*INV = A.
///* ***************************************************************************
boost::shared_ptr<Matrix> pseudoinverse(

  const Matrix& input )
{
  // if matrix is a square matrix pseudoinverse = inverse:
  if ( input.size1() == input.size2() )
  {
    return inverse(input);
  }
  // if matrix is not square calculate pseudoinverse:
  else
  {
    // declare working variables:
    int dim1( input.size1() ), dim2( input.size2() ), 
        min_dim( ( dim1 < dim2 )? dim1 : dim2 );

    // allocate memory for gsl objects:
    gsl_matrix *product = gsl_matrix_alloc( min_dim, min_dim );
    gsl_matrix *inverse = gsl_matrix_alloc( min_dim, min_dim );
    gsl_matrix *result = gsl_matrix_alloc( dim2, dim1 );
    gsl_permutation *permutation = gsl_permutation_alloc( min_dim );

    // calculate square product matrix input'input:
    if ( dim1 < dim2 ) 
      gsl_blas_dgemm( CblasNoTrans, CblasTrans, 1.0,  
                      input.get_gsl(), input.get_gsl(), 0.0, product );
    else 
      gsl_blas_dgemm( CblasTrans, CblasNoTrans, 1.0, 
                      input.get_gsl(), input.get_gsl(), 0.0, product );
          
    // invert the square matrix . (input'input)^-1:
    int signum;
    gsl_linalg_LU_decomp( product, permutation, &signum );
    gsl_linalg_LU_invert( product, permutation, inverse );
          
    // calculate pseudoinverse by multiplying (input'input)^-1 with input:
    if ( dim1 < dim2 ) 
      gsl_blas_dgemm( CblasTrans, CblasNoTrans, 1.0, 
                      input.get_gsl(), inverse, 0.0, result );
    else 
      gsl_blas_dgemm( CblasNoTrans, CblasTrans, 1.0, 
                      inverse, input.get_gsl(), 0.0, result );

    // free temporal variables.
    gsl_permutation_free( permutation );
    gsl_matrix_free( product );   
    gsl_matrix_free( inverse );    

    // done.
    return boost::shared_ptr<Matrix>( new Matrix( result ) );
  } 
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn Vector sphere2cart(
///* 	
///*     const Vector& input )
///* 
///* @brief Transforms a spherical-coordinate vector to cartesian-coordinates.
///* 
///* For further information view:                                          <br>
///*  Wikipedia article on cartesian and spherical coordinates.             <br>
///* 
///* @param Vector: a in spherical-coordinates 
///* 
///* @return Vector: represantation of a in cartesian-coordinates, order: x,y,z.
///* ***************************************************************************
boost::shared_ptr<Vector> sphere2cart(

  const Vector& input )
{
  if ( input.size() != 3 ) 
  throw std::range_error("Can only transform 3D-vectors.");
        
  boost::shared_ptr<Vector> result( new Vector(3) );

  (*result)(0) = input(0) * sin( input(1) ) * cos( input(2) );
  (*result)(1) = input(0) * sin( input(1) ) * sin( input(2) );
  (*result)(2) = input(0) * cos( input(1) );

  return result;
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn Vector QR_LS_solve(
///*  
///*     const Matrix& M
///*     const Vector& v )
///* 
///* @brief Performs a least square QR decomposition in order to solve Mx=v.
///* 
///* For further information view:                                          <br>
///*  Wikipedia article on QR decomposition and gsl documentation.          <br>
///* 
///* @param matrix: left side of equation.
///* @param Vector: right side of equation. 
///* 
///* @return Vector: solution of the equation in least square sense.
///* ***************************************************************************
boost::shared_ptr<Vector> QR_LS_solve(
    
  const Matrix& m,    
  const Vector& v )
{ 
  gsl_vector* x = gsl_vector_alloc( m.size2() );
  gsl_vector* tau = gsl_vector_alloc( m.size2() );
  gsl_vector* resid = gsl_vector_alloc( m.size1() );

  if ( gsl_linalg_QR_decomp( m.get_gsl(), tau ) ) 
    throw std::runtime_error( "Error during gsl: QR_decomp." );
  
  if ( gsl_linalg_QR_lssolve( m.get_gsl(), tau, v.get_gsl(), x, resid ) ) 
    throw std::runtime_error( "Error during gsl: QR_LS_solve." );
   
  gsl_vector_free( tau );
  gsl_vector_free( resid );
    
  return boost::shared_ptr<Vector>( new Vector( x ) );
}
//******************************************************************************



/// ****************************************************************************
///* @file math.cpp
///* @fn Vector cross3x3( 
///* 
///*     const Vector& a, 
///*     const Vector& b )
///* 
///* @brief Returns the cross product between the vectors a and b.
///* 
///* For further information view:                                          <br>
///*  Wikipedia article on cross product or just use google...
///* 
///* @param Vectors: a and b 
///* @return Vector: the cross product a x b
///* ***************************************************************************
boost::shared_ptr<Vector> cross3x3( 

  const Vector& a, 
  const Vector& b )
{
  if( a.size() != 3 || b.size() != 3 )
    throw std::range_error("Product only defined for 3D-vectors.");

  // Reserve memory for output variable.
  boost::shared_ptr<Vector> result( new Vector(3) );

  // Cross product calculation:
  (*result)(0) = a(1) * b(2) - a(2) * b(1);
  (*result)(1) = a(2) * b(0) - a(0) * b(2);
  (*result)(2) = a(0) * b(1) - a(1) * b(0);

  return result;
}
//******************************************************************************



/// ****************************************************************************
///* @file math.cpp
///* @fn void evd3x3(
///* 
///*     const Vector &d, 
///*     Vector &val, 
///*     Matrix &vec )
///* 
///* @brief Function for computing the eigenvalues and eigenvectors directly.
///* 
///* For further information view:                                          <br>
///*  Hasan et al. JMR 152, 41-47 (2001)
///* 
///* 
///* @param Diffusion-Tensor: d
///* @return Eigenvalues:  val
///* @return Eigenvectors: vec (columnwise)
///* ***************************************************************************
void evd3x3(

  const Vector &d, 
  Vector &val, 
  std::vector<Vector> &vec )
{
  if( d.size() != 6 )
    throw std::invalid_argument( "Tensor (6-component vector) expected!" );

  // calculate invariants:
  Vector c(3);          
  c(0) = d(0) + d(3) + d(5);
  
  c(1) = d(0)*d(3) + d(0)*d(5) + d(3)*d(5) 
       - d(1)*d(1) - d(2)*d(2) - d(4)*d(4);
       
  c(2) = d(0)*d(3)*d(5) + 2.0*d(1)*d(2)*d(4) 
       - d(5)*d(1)*d(1) - d(3)*d(2)*d(2) - d(0)*d(4)*d(4);

            // constants:
  const double v( c(0)*c(0)/9.0 - c(1)/3.0 );
  const double s( c(0)*c(0)*c(0)/27.0 - c(0)*c(1)/6.0 + c(2)/2.0 );
  const double phi( ( v > 0 && s*s < v*v*v )? 
                    acos( ( s / v ) * sqrt( 1.0 / v ) ) / 3 : 0.0 );
  
  // calculate eigenvalues:
  val = Vector(3);

  if( phi != 0 ) 
  {
    val(0) = c(0) / 3 + 2 * sqrt(v) * cos( phi );
    val(1) = c(0) / 3 - 2 * sqrt(v) * cos( M_PI / 3.0 + phi );
    val(2) = c(0) / 3 - 2 * sqrt(v) * cos( M_PI / 3.0 - phi );
  } 
  else val(0) = val(1) = val(2) = 0.0;

  // calculate eigenvectors:
  vec.reserve(3);

  for( unsigned long i(0); i < 3; i++ )
  {
    Vector tmp(3);
    // for each eigenvalue calculate the eigenvector:
    tmp(0) = ( d(1) * d(4) - ( d(3) - val(i) ) * d(2) ) 
           * ( d(2) * d(4) - ( d(5) - val(i) ) * d(1) );

    tmp(1) = ( d(2) * d(4) - ( d(5) - val(i) ) * d(1) ) 
           * ( d(2) * d(1) - ( d(0) - val(i) ) * d(4) );

    tmp(2) = ( d(1) * d(4) - ( d(3) - val(i) ) * d(2) ) 
           * ( d(2) * d(1) - ( d(0) - val(i) ) * d(4) );

             
    // normalization of eigenvectors:
    double norm( sqrt(tmp(0)*tmp(0)+tmp(1)*tmp(1)+tmp(2)*tmp(2)) );
    if( norm > 0.0 ) 
      for( unsigned long j(0); j < 3; j++ ) tmp(j) /= norm;
      
    else 
      for( unsigned long j(0); j < 3; j++ ) tmp(j) = 0.0;
    
    vec.push_back(tmp);
  }
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn Matrix sh_base(
///* 
///*     const Matrix& g,
///*     const int max_order )
///* 
///* @brief Calculates the spherical harmonics base for the given gradients
///* 
///*  For further information view:                                         <br>
///*  Maxime Descoteaux: PHD Thesis. 2008. page 64-67. 
///* 
///* @param Matrix: gradients 
///* @param Scalar: maximum spherical harmonic order
///* 
///* @return Matrix: the spherical harmonics base
///* ***************************************************************************
boost::shared_ptr<Matrix> sh_base(

    const Matrix& g,
    const int max_order )
{
  // allcoate result matrix
  unsigned long sh_dirs( (max_order + 2) * (max_order + 1) / 2 );
  boost::shared_ptr<Matrix> out( new Matrix( g.size1(), sh_dirs ) );

  // for each direction
  for( unsigned long i(0); i < g.size1(); i++ )
  {
    // transform current direction to polar coordinates
    double theta( acos( g(i,2) ) );
    double phi( atan2( g(i,1), g(i,0) ) );

    // calculate spherical harmonic base
    for( int order(0), j(0); order <= max_order; order+=2 )    
      for( int degree( -order ); degree <= order; degree++, j++ )
          (*out)(i,j) = sh_base_function( order, degree, theta, phi );                    
  }
  return out;
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn double sh_base_function(
///* 
///*     const int degree,
///*     const int order,
///*     const double theta,
///*     const double phi )
///* 
///* @brief Defines the spherical harmonic base function used.
///* 
///* For further information view:                                          <br>
///*  Maxime Descoteaux: PHD Thesis. 2008. page 64-67.                      <br>
///*  itkRealSymSphericalHarmonicBasis.h as documented by Luke Bloy, 12/09/2009
///* 
///* @param Scalar: order of the SH 
///* @param Scalar: degree of the SH
///* @param Scalar: angle theta
///* @param Scalar: angle phi
///* 
///* @return Scalar: the value of the spherical harmonic
///* ***************************************************************************
double sh_base_function(

  const int order,
  const int degree,
  const double theta,
  const double phi )
{
  // library for spherical harmonics and legendre polynomials.
  using namespace boost::math;
  
  double P = gsl_sf_legendre_sphPlm( order, abs(degree), cos(theta) );
  
  if( degree > 0 )
    return P * cos( degree * phi );
    //spherical_harmonic_r( order, fabs(degree), theta, phi );
  
  else if( degree < 0 )
    return P * sin( -degree * phi );
    //spherical_harmonic_i( order, fabs(degree), theta, phi );
  
  else
    return P;//spherical_harmonic_r( order, 0, theta, phi );
}
//******************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* @fn double sh_eval(
///* 
///*     const Vector& position,
///*     const Vector& coeff,
///*     const unsigned long max_order )
///* 
///* @brief Evaluates a spherical harmonic function defined by its coefficients 
///* in direction of the position vector.
///* 
///* important: this function works in place.                               <br>
///* 
///* For further information view:                                          <br>
///*  Cormen, Leiversn, Rivest, and Stein: Introduction to Algorithms       <br>
///*  Wikipedia article on bubble sort.
///* 
///* @param Vector: values the vector which determies the sorting
///* @param Vector: indices to be sorted going along 
///* 
///* @return TODO
///* ***************************************************************************
double sh_eval(

  const Vector& position, //in spherical coordinates
  const Vector& coeff )
{
  const int max_order((-3+static_cast<int>(sqrt(8*coeff.size())+1))/2);
  
  const double radius( position(0) );
  if( radius == 0.0 ) return 0.0;
  
  // for easier readability
  const double theta( position(1) );
  const double phi( position(2) );

  double result(0);
  for( int order(0), j(0); order <= max_order; order+=2 )    
    for( int degree( -order ); degree <= order; degree++, j++ ) 
      result += coeff(j) * sh_base_function( order, degree, theta, phi );

  return result;
}    
//******************************************************************************


///* ***************************************************************************
///* @file Extrema.cpp
///* @fn size_vectorT local_maxima(
///*     const matrixT& adjacent,
///*     const vectorT& values )
///* 
///* @brief Returns the local maxima on the tesselation of a sphere. 
///* 
///* This is implemented using the formal definition of a local maxima, 
///* i.e. value which is larger than the value of any (directly) adjacent value.
///* We use the discretization grid for localizing the maxima by comparison
///* to the adjacent values.  
///* For further information view:
///*  Wikipedia article on maxima or just use google...
///* 
///* @param Matrix: Adjacency matrix(contains the neighbors of point i in row i)
///* @param Vector: Values at the differnt points.
///* 
///* 
///* @return Vector: Indicees of the maximum points.
///* ***************************************************************************
boost::shared_ptr< std::vector<unsigned long> >local_maxima(    
  const Matrix& adjacency,
  const Vector& values )
{
  // reserve result array:
  boost::shared_ptr< std::vector<unsigned long> > 
    result( new std::vector<unsigned long> );

  // set all values to unchecked:
  std::vector<bool> checked( values.size(), false );

  for( unsigned long i( 0 ); i < values.size(); i++ )
  {
    // if value was not checked in context of a different value:
    if( !checked[i] )
    {
      checked[i] = true;
      
      // traverse the adjacent values until larger value is found.
      bool maximum( true );
      if( values(i)<=0.2 ) maximum = false;
      else for( unsigned long j(0); (j < 6) && (adjacency(i,j) > -1) 
                                            && maximum; j++ )
      {
          if( values(adjacency(i,j))>values(i) )
          {
            maximum = false;
          }
          else checked[ adjacency(i,j) ] = true;
      }
      
      // if local maximum is found save value.
      if( maximum ) (*result).push_back(i);
    }
  }
  return result; 
}
//* ****************************************************************************


///* ***************************************************************************
///* @file Extrema.cpp
///* @fn bool is_local_min(
///*   const Vector& values,
///*   const Matrix& adjacency,
///*   unsigned long i )
///* 
///* @brief Returns the if the given index belongs to a local minimum. 
///* 
///* ***************************************************************************
bool is_local_min(
  const Vector& values,
  const Matrix& adjacency,
  unsigned long i )
{
  for( unsigned long j(0); (j < 6) && (adjacency(i,j) > -1); j++ )
    if( values(adjacency(i,j))<values(i) ) return false;

  return true;
}
//* ****************************************************************************

///* ***************************************************************************
///* @file math.cpp
///* @fn imageT ranked_maxima(
///* 
///*     const imageT& sh_data,
///*     const sizeT steps,
///*     const sizeT order,
///*     const rangeS view )
///* 
///* @brief TODO
///* 
///* @param TODO
///* @return TODO
///* ***************************************************************************
boost::shared_ptr<Vector> ranked_maxima(
  const Vector& radius,
  const Matrix& tesselation,
  const Matrix& adjacency,
  const unsigned long num_max )
{
  // find maxima:
  boost::shared_ptr< std::vector<unsigned long> > 
    index( local_maxima( adjacency, radius ) );
  
  Vector maxima( (*index).size() );
  for( unsigned long i(0); i < (*index).size();  i++ )
    maxima( i ) = radius( (*index)[i] );
  
  // save sorted maxima:
  bubble_sort( maxima, (*index) );
  
  boost::shared_ptr<Vector> result( new Vector(num_max*3,true) );
  
  // only the num_max largest maxima are saved:
  for( unsigned long i(0); i < (*index).size()/2 && i < num_max; i++ )
  {
    (*result)(i*3+0) = tesselation( (*index)[2*i],0); 
    (*result)(i*3+1) = tesselation( (*index)[2*i],1); 
    (*result)(i*3+2) = tesselation( (*index)[2*i],2);
  }
  
  return result;
}
//* ****************************************************************************


///* ***************************************************************************
///* @file math.cpp
///* @fn imageT ranked_maxima(
///* 
///*     const imageT& sh_data,
///*     const sizeT steps,
///*     const sizeT order,
///*     const rangeS view )
///* 
///* @brief TODO
///* 
///* @param TODO
///* @return TODO
///* ***************************************************************************
boost::shared_ptr< std::vector<unsigned long> > ranked_maxima_index(
  const Vector& radius,
  const Matrix& adjacency,
  const unsigned long num_max )
{
  // find maxima:
  boost::shared_ptr< std::vector<unsigned long> > 
    index( local_maxima( adjacency, radius ) );
    
  Vector maxima( (*index).size() );
  for( unsigned long i(0); i < (*index).size();  i++ )
    maxima( i ) = radius( (*index)[i] );

  // save sorted maxima:
  bubble_sort( maxima, (*index) );
  
  unsigned long size = ((*index).size()<num_max*2)? (*index).size() : num_max*2;
  boost::shared_ptr< std::vector<unsigned long> > 
    result(new std::vector<unsigned long>(size));
  
  // only the five largest maxima are saved:
  for( unsigned long i(0); i < size; i++ )
  {
    (*result)[i] = (*index)[i]; 
  }

  return result;
}
//* ****************************************************************************


///* ***************************************************************************
///* @file math.cpp
///* 
///* @brief TODO
///* 
///* @param TODO
///* @return TODO
///* ***************************************************************************
boost::shared_ptr<Vector> moment_of_inertia(

    const Vector& values,
    const std::vector<Vector>& points )
{
    boost::shared_ptr<Vector> result( new Vector(6,true) );
    
    double sum(0.0);
    for( unsigned long i(0); i < points.size(); i++ )
    {
        double x( points[i](0) );
        double y( points[i](1) );
        double z( points[i](2) );
        
        (*result)(0) += values(i) * x * x;
        (*result)(1) += values(i) * x * y;
        (*result)(2) += values(i) * x * z;
        (*result)(3) += values(i) * y * y;
        (*result)(4) += values(i) * y * z;
        (*result)(5) += values(i) * z * z;
        
        sum += fabs(values(i));
    }
    (*result) = (*result)/sum;
    return result;
}
//* ****************************************************************************



///* ***************************************************************************
///* @file math.cpp
///* 
///* @brief TODO
///* 
///* @param TODO
///* @return TODO
///* ***************************************************************************
boost::shared_ptr<Vector> fit_bingham(
  const Vector& sh_data,
  const Matrix& tesselation,
  const Matrix& adj,
  const Matrix& base,
  const unsigned long neighborhood,
  const unsigned long num_max )

{  
  
  unsigned int mod = 14;
  
  // reserve memory:
  boost::shared_ptr<Vector> result( new Vector(num_max*mod,true) );
  
  if( sh_data(0) == 0 ) return result;
  
  Vector radius( prod( base, sh_data ) );
  
  // get maxima:
  boost::shared_ptr< std::vector<unsigned long> > 
    max( ranked_maxima_index( radius, adj, num_max ));
    

    
  for(unsigned long n_max(0);(n_max<(*max).size()/2)&&(n_max<num_max); n_max++)
  {
    // build neighborhood matrix for both matrix halves:
    std::vector<unsigned long> g, h, index;
    g.reserve(adj.size1()); h.reserve(adj.size1()); index.resize(adj.size1(),0);
    
    // initialize lists with corresponding maximum:
    g.push_back((*max)[2*n_max]); h.push_back((*max)[2*n_max+1]);
    index[(*max)[2*n_max]] = 1; index[(*max)[2*n_max+1]] = 1;
    
    for( unsigned long i(0),gi(0),hi(0); i < neighborhood; i++ )
    {
      // for first maximum:
      for( unsigned long j(g.size()); gi < j; gi++ )
        for( unsigned long k(0); k < 6 && adj( g[gi], k ) > 0; k++ )
          if( !index[ adj( g[gi], k ) ] ) 
          {
            g.push_back( adj( g[gi], k ) );   
            index[ adj( g[gi], k ) ] = 1;
            if( i == neighborhood-1 )
            {
              Vector cur(3);
              cur(0) = tesselation(adj( g[gi], k ),0);
              cur(1) = tesselation(adj( g[gi], k ),1);
              cur(2) = tesselation(adj( g[gi], k ),2); 
              
              Vector maxV(3);
              maxV(0) = tesselation((*max)[2*n_max],0);
              maxV(1) = tesselation((*max)[2*n_max],1);
              maxV(2) = tesselation((*max)[2*n_max],2); 
            }
          }
      // for opposite maximum:    
      for( unsigned long j(h.size()); hi < j; hi++ )
        for( unsigned long k(0); k < 6 && adj( h[hi], k ) > 0; k++ )
          if( !index[ adj( h[hi], k ) ] ) 
          {
            h.push_back( adj( h[hi], k ) );
            index[ adj( h[hi], k ) ] = 1;
          }
    }
    // attach lists of neighbors for g and h:
    for( unsigned long i(0); i < h.size(); i++ ) g.push_back( h[i] );
    
    // preprocessing for moment of inertia matrix:
    Vector values( g.size() );
    std::vector<Vector> points;  
    Vector maxV(3);
    maxV(0) = tesselation( g[0], 0 );
    maxV(1) = tesselation( g[0], 1 );
    maxV(2) = tesselation( g[0], 2 );
      
    for( unsigned long i(0); i < g.size(); i++ )
    {
      Vector cur(3);
      cur(0) = tesselation( g[i], 0 );
      cur(1) = tesselation( g[i], 1 );
      cur(2) = tesselation( g[i], 2 );
      
      if( iprod( cur, maxV ) < 0 ) cur = cur * (-1);
      points.push_back(cur);
      
      cur = *cart2sphere(cur);
      values(i) = sh_eval( cur, sh_data );
    }
    // calculate moment_of_inertia and extract values:
    boost::shared_ptr<Vector>  m_inertia( moment_of_inertia(values,points) );    
    std::vector<Vector> vecs; Vector vals(3);

    // the eigenvectors are the bingham parameter mu:
    evd3x3( *m_inertia, vals, vecs );
    
    // copies of these vectors in spherical coordinates:
    Vector z0(*cart2sphere(vecs[0]));
    Vector z1(*cart2sphere(vecs[1]));
    Vector z2(*cart2sphere(vecs[2]));
    // get function value at maximum:
    double f0 = sh_eval( z0, sh_data );
    
    g.clear(); h.clear(); index.clear();
    g.reserve(adj.size1()); h.reserve(adj.size1()); index.resize(adj.size1(),0);
    
    // initialize lists with corresponding maximum:
    g.push_back((*max)[2*n_max]); h.push_back((*max)[2*n_max+1]);
    index[(*max)[2*n_max]] = 1; index[(*max)[2*n_max+1]] = 1;
    
    double angle = 0.0;
    unsigned long maxI = 0;
    
    for( unsigned long i(0),gi(0),hi(0); //i < neighbourhood &&
         gi < g.size() && hi < h.size(); i++ )
    {
      // for first maximum:
      for( unsigned long j(g.size()); gi < j; gi++ )
        for( unsigned long k(0); k < 6 && adj( g[gi], k ) > 0; k++ )
          if( !index[ adj( g[gi], k ) ] ) 
          {
            if( radius(g[gi]) > radius(adj(g[gi],k)) 
             && radius(adj(g[gi],k)) > 0.0 )
            {
              g.push_back( adj( g[gi], k ) );
              if( i > maxI )
              {
                maxI = i;
                
                Vector cur(3);
                cur(0) = tesselation(adj( g[gi], k ),0);
                cur(1) = tesselation(adj( g[gi], k ),1);
                cur(2) = tesselation(adj( g[gi], k ),2); 
                
                angle = 360.0 * acos( iprod( cur, vecs[0] ) )/(2.*M_PI);
              }
            }
            
            index[ adj( g[gi], k ) ] = 1;
          }
      // for opposite maximum:    
      for( unsigned long j(h.size()); hi < j; hi++ )
        for( unsigned long k(0); k < 6 && adj( h[hi], k ) > 0; k++ )
          if( !index[ adj( h[hi], k ) ] ) 
          {
            if( radius(h[hi]) > radius(adj(h[hi],k)) 
             && radius(adj(h[hi],k)) > 0.0 )
            {
              h.push_back( adj( h[hi], k ) );
              if( i > maxI )
              {
                maxI = i;
                
                Vector cur(3);
                cur(0) = tesselation(adj( h[hi], k ),0);
                cur(1) = tesselation(adj( h[hi], k ),0);
                cur(2) = tesselation(adj( h[hi], k ),0); 
                
                angle = 360.0 * acos( iprod( cur, vecs[0] ) )/(2.*M_PI);
              }
            }
            
            index[ adj( h[hi], k ) ] = 1;
          }
    }
    
    // attach lists of neighbors for g and h:
    for( unsigned long i(0); i < h.size(); i++ ) g.push_back( h[i] );  
    
    if( g.size() > 2 )
    {
      
      // reserve calculation variables:
      Matrix A(g.size(),2,true);
      Vector b(g.size(),true);

      // build matrix for least square solution:
      for( unsigned long i(0); i < g.size(); i++ )
      {
        Vector cur(3);
        cur(0) = tesselation(g[i],0);
        cur(1) = tesselation(g[i],1);
        cur(2) = tesselation(g[i],2);

        double f(sh_eval(*cart2sphere(cur),sh_data));
        
        if( f0 > f )
        {
          A(i,0) = -iprod(vecs[1],cur)*iprod(vecs[1],cur);
          A(i,1) = -iprod(vecs[2],cur)*iprod(vecs[2],cur);
          b(i) = log(f/f0);
        }
      }
      
      Vector k_s( prod( *pseudoinverse( A ), b ) );

      if( k_s(0) > 0.0 || k_s(1) > 0.0 )
      {
        // order accordingly:
        if( k_s(0) > k_s(1) )
        {
          Vector tmp_v( vecs[1] ); vecs[1] = vecs[2]; vecs[2] = tmp_v;
          double tmp_d( k_s(1) );  k_s(1) = k_s(0);   k_s(0) = tmp_d;
        }
        
        // format output:
        (*result)(n_max*mod+0) = vecs[0](0); 
        (*result)(n_max*mod+1) = vecs[0](1); 
        (*result)(n_max*mod+2) = vecs[0](2);
        
        (*result)(n_max*mod+3) = vecs[1](0); 
        (*result)(n_max*mod+4) = vecs[1](1); 
        (*result)(n_max*mod+5) = vecs[1](2);
        
        (*result)(n_max*mod+6) = vecs[2](0); 
        (*result)(n_max*mod+7) = vecs[2](1); 
        (*result)(n_max*mod+8) = vecs[2](2);
      
        (*result)(n_max*mod+9)  = k_s(0);
        (*result)(n_max*mod+10) = k_s(1);
        
        (*result)(n_max*mod+11) = f0; 
        
        (*result)(n_max*mod+12) = maxI; 
        (*result)(n_max*mod+13) = angle; 
      }
      else
      {
        // format output:
        (*result)(n_max*mod+0) = 0.0; 
        (*result)(n_max*mod+1) = 0.0; 
        (*result)(n_max*mod+2) = 0.0;
        
        (*result)(n_max*mod+3) = 0.0; 
        (*result)(n_max*mod+4) = 0.0; 
        (*result)(n_max*mod+5) = 0.0;
        
        (*result)(n_max*mod+6) = 0.0; 
        (*result)(n_max*mod+7) = 0.0; 
        (*result)(n_max*mod+8) = 0.0;
      
        (*result)(n_max*mod+9)  = 0.0;
        (*result)(n_max*mod+10) = 0.0;
        
        (*result)(n_max*mod+11) = 0.0;
        
        (*result)(n_max*mod+12) = 0.0; 
        (*result)(n_max*mod+13) = 0.0; 
      }
    }
  }
  
  return result; 
}  
//* ****************************************************************************
