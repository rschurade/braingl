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

#include "math_structs.hpp"

Vector operator+ ( const Vector& v1, const Vector& v2 )
{
    Vector result( v1 );
    
    if( gsl_vector_add ( result.get_gsl(), v2.get_gsl() ) ) 
      throw std::runtime_error( "GSL vector Runtime Error!" );
    
    return result;
}



Vector operator- ( const Vector& v1, const Vector& v2 )
{
    Vector result( v1 );
    
    if( gsl_vector_sub ( result.get_gsl(), v2.get_gsl() ) ) 
      throw std::runtime_error( "GSL vector Runtime Error!" );
    
    return result;
}



Vector operator* ( const Vector& v1, const double s )
{
    Vector result( v1 );
    
    if( gsl_vector_scale ( result.get_gsl(), s ) ) 
      throw std::runtime_error( "GSL vector Runtime Error!" );
    
    return result;
}



Vector operator* ( const double s, const Vector& v1 )
{
    return v1 * s;
}



Vector operator/ ( const Vector& v1, const double s )
{
    return v1 * (1.0/s);
}



Matrix operator+ ( const Matrix& m1, const Matrix& m2 )
{
    Matrix result( m1 );
    
    if( gsl_matrix_add( result.get_gsl(), m2.get_gsl() ) )
      throw std::runtime_error( "GSL matrix Runtime Error!" );
    
    return result;
}



Matrix operator- ( const Matrix& m1, const Matrix& m2 )
{
    Matrix result( m1 );
    
    if( gsl_matrix_sub( result.get_gsl(), m2.get_gsl() ) )
      throw std::runtime_error( "GSL matrix Runtime Error!" );
    
    return result;
}



Matrix operator* ( const Matrix& m1, const double s )
{
    Matrix result( m1 );
    
    if( gsl_matrix_scale( result.get_gsl(), s ) ) 
      throw std::runtime_error( "GSL matrix Runtime Error!" );
    
    return result;
}



Matrix operator* ( const double s, const Matrix& m1 )
{
    return m1 * s;
}



Matrix operator/ ( const double s, const Matrix& m1 )
{
    return m1 * (1.0/s);
}


//* ****************************************************************************
//* ALGEBRA
//* ****************************************************************************


std::ostream& operator<< ( std::ostream& stream, const Vector& v )
{
    stream << "[" << v.size() << "]"<<"(";
    for ( unsigned long i(0); i < v.size(); i++ ) stream << v(i) << " ";
    stream <<")";
    
    return stream;
}



std::ostream& operator<< ( std::ostream& stream, const Matrix& m )
{
    stream << "[" << m.size1() << "," << m.size2() << "]"<< "(";
    for( unsigned long i(0); i < m.size1(); i++ )
    {
        stream << "(";
        for( unsigned long j(0); j < m.size2(); j++ ) stream << m(i,j) << " ";
        stream << ")";
    }
    stream << ")";
    
    return stream;
}



Matrix trans( const Matrix& m )
{
    if( !m.get_gsl() ) return Matrix();
    else
    {
        Matrix result( m.size2(), m.size1() );
        for( unsigned long i(0); i < m.size1(); i++ )
            for( unsigned long j(0); j < m.size2(); j++ )
                result(j,i) = m(i,j);
            
        return result;
    }
}



Matrix prod( const Matrix& m1, const Matrix& m2 )
{
    gsl_matrix* result = gsl_matrix_alloc( m1.size1(), m2.size2() );
    
    if( gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, 
                        m1.get_gsl(), m2.get_gsl(), 0.0, result ) )
        throw std::runtime_error( "GSL matrix Runtime Error!" );
    
    return Matrix( result );
}



Vector prod( const Matrix& m, const Vector& v )
{
    gsl_vector* result = gsl_vector_alloc( m.size1() );
    
    if( gsl_blas_dgemv(CblasNoTrans,1.0, m.get_gsl(), v.get_gsl(),0.0, result))
        throw std::runtime_error( "GSL matrix Runtime Error!" );
    
    return Vector( result );
}



Vector prod( const Vector& v, const Matrix& m  )
{
    gsl_vector* result = gsl_vector_alloc( m.size1() );
    
    if( gsl_blas_dgemv( CblasTrans, 1.0, m.get_gsl(), v.get_gsl(), 0.0, result ))
        throw std::runtime_error( "GSL matrix Runtime Error!" );
    
    return Vector( result );
}



double  iprod( const Vector& v1, const Vector& v2 )
{
    double result(0.0);
    
    if( v1.size() !=  v2.size() ) 
      throw std::range_error( "Vectors in scalar product need same size!" );
    
    for( unsigned long i(0); i < v1.size(); i++ ) result += v1(i) * v2(i);
    
    return result;
}
