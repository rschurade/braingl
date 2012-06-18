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

#ifndef INC_MATHSTRUCTS_H
#define INC_MATHSTRUCTS_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdexcept>

//GSL
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_sf_hyperg.h>
#include <gsl/gsl_sf_legendre.h>

//* ****************************************************************************
//* VECTOR
//* ****************************************************************************

class Vector{

private:
  gsl_vector *_V;


public:
  // constructors:
  Vector();
  Vector( const Vector& v );
  Vector( gsl_vector* v );
  Vector( const unsigned long size, const bool zero=false );
  ~Vector();

  // operators:
  double operator() ( const unsigned long pos ) const;
  double & operator() ( const unsigned long pos );
  Vector& operator= ( const Vector& v );
  Vector& operator= ( gsl_vector* v );
  Vector& operator+= ( const Vector& v );
  Vector& operator+= ( gsl_vector* v );
  Vector& operator-= ( const Vector& v );
  Vector& operator-= ( gsl_vector* v );

  // functions:
  const unsigned long size() const;

  gsl_vector* get_gsl() const;
  void set_gsl( gsl_vector* v );

  const double norm() const;

  friend std::ostream& operator<< ( std::ostream& stream, const Vector& v );
};

//* ****************************************************************************
//* MATRIX
//* ****************************************************************************

class Matrix{

private:
  gsl_matrix *_M;

public:
  // constructors:
  Matrix();
  Matrix( const Matrix& m );
  Matrix( gsl_matrix* m );
  Matrix( const unsigned long size1, 
          const unsigned long size2, 
          const bool zero=false );
  
  ~Matrix();

  // operators:
  double & operator() ( const unsigned long pos1, 
                        const unsigned long pos2 );
  
  double operator() ( const unsigned long pos1, 
                      const unsigned long pos2 ) const;                        
                        
  Matrix& operator= ( const Matrix& m );
  Matrix& operator= ( gsl_matrix* m );
  Matrix& operator+= ( const Matrix& m );
  Matrix& operator+= ( gsl_matrix* m );
  Matrix& operator-= ( const Matrix& m );
  Matrix& operator-= ( gsl_matrix* m );

  // functions:
  const unsigned long size1() const;
  const unsigned long size2() const;

  gsl_matrix* get_gsl() const;
  void set_gsl( gsl_matrix* m );

  friend std::ostream& operator<< ( std::ostream& stream, const Matrix& m );
};

//* ****************************************************************************
//* ALGEBRA
//* ****************************************************************************

Vector operator+ ( const Vector& v1, const Vector& v2 );
Vector operator- ( const Vector& v1, const Vector& v2 );
Vector operator* ( const Vector& v1, const double s );
Vector operator* ( const double s, const Vector& v1 );
Vector operator/ ( const Vector& v1, const double s );

Matrix operator+ ( const Matrix& m1, const Matrix& m2 );
Matrix operator- ( const Matrix& m1, const Matrix& m2 );
Matrix operator* ( const Matrix& m1, const double s );
Matrix operator* ( const double s, const Matrix& m1 );
Matrix operator/ ( const double s, const Matrix& m1 );

Matrix trans( const Matrix& m );
Matrix prod( const Matrix& m1, const Matrix& m2 );
Vector prod( const Matrix& m, const Vector& v );
Vector prod( const Vector& v, const Matrix& m );
double  iprod( const Vector& v1, const Vector& v2 );


//* ****************************************************************************
//* VECTOR
//* ****************************************************************************

inline Vector::Vector()
{
  _V = NULL;
}



inline Vector::Vector( const Vector& v )
{
  if ( v.size() )
  {
    _V = gsl_vector_alloc( v.size() );
    gsl_vector_memcpy( _V, v.get_gsl() );
  }
  else
  {
    _V = NULL;
  }
}



inline Vector::Vector( gsl_vector* v )
{
  //takes ownership of gsl pointer
  _V = v;
}



inline Vector::Vector( const unsigned long size, const bool zero )
{
  _V = ( size ) ? gsl_vector_alloc( size ) : NULL;
  if ( size && zero ) gsl_vector_set_zero( _V );
}



inline Vector::~Vector()
{
  if ( _V ) gsl_vector_free( _V );
}

//* ****************************************************************************

inline double& Vector::operator() ( const unsigned long pos )
{
  return _V->data[ pos ];
}

inline double Vector::operator() ( const unsigned long pos ) const
{
  return _V->data[ pos ];
}

inline Vector& Vector::operator= ( const Vector& v )
{
  if ( this )
  {
    if ( _V == v._V ) return *this;
    if ( _V ) gsl_vector_free( _V );
  }
  _V = gsl_vector_alloc( v.size() );
  gsl_vector_memcpy( _V, v._V );
  
  return *this;
}



inline Vector& Vector::operator= ( gsl_vector* v )
{
  if ( this )
  {
    if ( _V == v ) return *this;
    if ( _V ) gsl_vector_free( _V );
  }
  _V = gsl_vector_alloc( v->size );
  gsl_vector_memcpy( _V, v );
  
  return *this;
}



inline Vector& Vector::operator+= ( const Vector& v )
{
  if ( !v._V || !_V ) 
    throw std::out_of_range("+= is not defined for uninitialized structures.");
  
  else if ( gsl_vector_add ( _V, v._V ) ) 
    throw std::runtime_error( "GSL vector Runtime Error!" );
  
  return *this;
}



inline Vector& Vector::operator+= ( gsl_vector* v )
{
  if ( !v || !_V ) 
    throw std::out_of_range("+= is not defined for uninitialized structures.");
  
  else if ( gsl_vector_add ( _V, v ) ) 
    throw std::runtime_error( "GSL vector Runtime Error!" );
  
  return *this;
}



inline Vector& Vector::operator-= ( const Vector& v )
{
  if ( !v._V || !_V ) 
    throw std::out_of_range("-= is not defined for uninitialized structures.");
  
  else if ( gsl_vector_sub ( _V, v._V ) ) 
    throw std::runtime_error( "GSL vector Runtime Error!" );
  
  return *this;
}



inline Vector& Vector::operator-= ( gsl_vector* v )
{
  if ( !v || !_V ) 
    throw std::out_of_range("-= is not defined for uninitialized structures.");
  
  else if ( gsl_vector_sub ( _V, v ) ) 
    throw std::runtime_error( "GSL vector Runtime Error!" );
  
  return *this;
}

//* ****************************************************************************

inline const unsigned long Vector::size() const
{
  if ( _V ) return _V->size;
  else return 0;
}



inline gsl_vector* Vector::get_gsl() const
{
  return _V;
}



inline void Vector::set_gsl( gsl_vector* v)
{
  if ( _V ) gsl_vector_free( _V );
  _V = v;
}



inline const double Vector::norm() const
{
  if ( !_V ) return 0.0;
  else
  {
    double result( 0.0 );
    
    for( unsigned long i(0); i < _V->size; i++  ) 
      result += (*this)(i) * (*this)(i);
    
    return sqrt(result);
  }
}



//* ****************************************************************************
//* MATRIX
//* ****************************************************************************

inline Matrix::Matrix()
{
  _M = NULL;
}



inline Matrix::Matrix( const Matrix& m )
{
  if ( m.size1() && m.size2() )
  {
    _M = gsl_matrix_alloc( m.size1(), m.size2() );
    gsl_matrix_memcpy( _M, m.get_gsl() );
  }
  else
  {
    _M = NULL;
  }
}



inline Matrix::Matrix( gsl_matrix* m )
{
  //takes ownership of gsl pointer
  _M = m;
}



inline Matrix::Matrix( 
  const unsigned long size1, 
  const unsigned long size2, 
  const bool zero )
{
  _M = ( size1 && size2 ) ? gsl_matrix_alloc( size1, size2 ) : NULL;
  if ( size1 && size2 && zero ) gsl_matrix_set_zero( _M );
}



inline Matrix::~Matrix()
{
  if ( _M ) gsl_matrix_free( _M );
}

//* ****************************************************************************

inline double &Matrix::operator() ( const unsigned long pos1, 
                                     const unsigned long pos2 )
{
  return _M->data[pos1 * _M->tda + pos2];
}



inline double Matrix::operator() ( const unsigned long pos1, 
                                     const unsigned long pos2 ) const
{
  return _M->data[pos1 * _M->tda + pos2];
}



inline Matrix& Matrix::operator= ( const Matrix& m )
{ 
  if ( _M == m._M ) return *this;
  if ( _M ) gsl_matrix_free( _M );

  _M = gsl_matrix_alloc( m.size1(), m.size2() );
  gsl_matrix_memcpy( _M, m._M );
  
  return *this;
}



inline Matrix& Matrix::operator= ( gsl_matrix* m )
{
  if ( _M == m ) return *this;
  if ( _M ) gsl_matrix_free( _M );

  _M = gsl_matrix_alloc( m->size1, m->size2 );
  gsl_matrix_memcpy( _M, m );
  
  return *this;
}



inline Matrix& Matrix::operator+= ( const Matrix& m )
{
  if ( !m._M || !_M ) 
    throw std::out_of_range("+= is not defined for uninitialized structures.");
  
  else if( gsl_matrix_add ( _M, m._M ) )
    throw std::runtime_error( "GSL matrix Runtime Error!" );
  
  return *this;
}



inline Matrix& Matrix::operator+= ( gsl_matrix* m )
{
  if ( !m || !_M ) 
    throw std::out_of_range("+= is not defined for uninitialized structures.");
  
  else if( gsl_matrix_add ( _M, m ) ) 
    throw std::runtime_error( "GSL matrix Runtime Error!" );
  
  return *this;
}



inline Matrix& Matrix::operator-= ( const Matrix& m )
{
  if( !m._M || !_M ) 
    throw std::out_of_range("-= is not defined for uninitialized structures.");
  
  else if( gsl_matrix_add ( _M, m._M ) ) 
    throw std::runtime_error( "GSL matrix Runtime Error!" );
  
  return *this;
}



inline Matrix& Matrix::operator-= ( gsl_matrix* m )
{
  if( !m || !_M ) 
    throw std::out_of_range("-= is not defined for uninitialized structures.");
  
  else if( gsl_matrix_add ( _M, m ) ) 
    throw std::runtime_error( "GSL matrix Runtime Error!" );
  
  return *this;
}

//* ****************************************************************************

inline const unsigned long Matrix::size1() const
{
  if( _M ) return _M->size1;
  else return 0;
}



inline const unsigned long Matrix::size2() const
{
  if( _M ) return _M->size2;
  else return 0;
}



inline gsl_matrix* Matrix::get_gsl() const
{
  return _M;
}



inline void Matrix::set_gsl( gsl_matrix* m )
{
  if( _M ) gsl_matrix_free( _M );
  _M = m;
}

//* ****************************************************************************

#endif /* INC_MATHSTRUCTS_H */
