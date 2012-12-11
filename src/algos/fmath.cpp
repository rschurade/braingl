/*
 * fmath.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: schurade
 */
#include <QtCore/QDebug>

#include <math.h>

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "../thirdparty/newmat10/newmatap.h"
#include "../thirdparty/newmat10/newmatio.h"
#include "../thirdparty/newmat10/newmatrm.h"
#include "../thirdparty/newmat10/precisio.h"


#include "fmath.h"

FMath::FMath() {}
FMath::~FMath() {}

ColumnVector FMath::vlog( ColumnVector v )
{
    // allocate memory of output object:
    ColumnVector tmp( v.Nrows() );

    // for each element of the vector:
    for ( int i( 1 ); i <= v.Nrows(); ++i )
    {
        if ( v( i ) <= 0.0 )
            tmp( i ) = -1.0e+17;
        else
            tmp( i ) = log( v( i ) );
    }
    return tmp;
}

double FMath::legendre_p( int k )
{
    double z = 1.0;
    double n = 1.0;

    for ( int i = 1; i <= k + 1; i += 2 ) z *= i;
    for ( int i = 2; i <= k - 2; i += 2 ) n *= i;

    if (( k / 2 ) % 2 == 0)
    {
        return -1.0 / ( 8 * M_PI ) * ( z / n );
    }
    else
    {
        return 1.0 / ( 8 * M_PI ) * ( z / n );
    }

}

Matrix FMath::sh_base( Matrix g, int maxOrder )
{
    //qDebug() << "start calculating sh base";
    // allcoate result matrix
    unsigned long sh_dirs( ( maxOrder + 2 ) * ( maxOrder + 1 ) / 2 );
    Matrix out( g.Nrows(), sh_dirs );
    out = 0.0;

    // for each direction
    for ( int i = 0; i < g.Nrows(); ++i )
    {
        // transform current direction to polar coordinates
        double theta( acos( g( i + 1, 3 ) ) );
        double phi( atan2( g( i + 1, 2 ), g( i + 1, 1 ) ) );

        // calculate spherical harmonic base
        for ( int order = 0, j = 0; order <= maxOrder; order += 2 )
        {
            for ( int degree( -order ); degree <= order; ++degree, ++j )
            {
                out( i + 1, j + 1 ) = sh_base_function( order, degree, theta, phi );
            }
        }
    }
    //qDebug() << "finished calculating sh base";
    return out;
}

double FMath::sh_base_function( int order, int degree, double theta, double phi )
{
    using namespace boost::math;
#if 0
    double P = legendre_p<double>( order, abs(degree), cos(theta) );

    if ( degree > 0 )
    {
        return P * cos( degree * phi );
    }
    else if ( degree < 0 )
    {
        return P * sin( -degree * phi );
    }
    else
    {
        return P;
    }
#else
    if ( degree > 0 )
    {
        return spherical_harmonic_r( order, abs( degree ), theta, phi );
    }
    else if ( degree < 0 )
    {
        return spherical_harmonic_i( order, abs( degree ), theta, phi );
    }
    else
    {
        return spherical_harmonic_r( order, 0, theta, phi );
    }
#endif
}

ColumnVector FMath::moment_of_inertia( const ColumnVector& values, const QVector<ColumnVector>& points )
{
    ColumnVector result( 6 );

    double sum( 0.0 );
    for ( int i = 0; i < points.size(); ++i )
    {
        double x( points[i]( 1 ) );
        double y( points[i]( 2 ) );
        double z( points[i]( 3 ) );
        double val = fabs( values( i + 1 ) );
//        qDebug() << x << y << z;
//        qDebug() << "val" << val;

        result( 1 ) += x * x * val;
        result( 2 ) += x * y * val;
        result( 3 ) += x * z * val;
        result( 4 ) += y * y * val;
        result( 5 ) += y * z * val;
        result( 6 ) += z * z * val;

        sum += val * val;
    }
    result = result / sum;
    //qDebug() << result( 1 ) << result( 2 ) << result( 3 ) << result( 4 ) << result( 5 ) << result( 6 );

    return result;
}

double FMath::iprod( const ColumnVector& v1, const ColumnVector& v2 )
{
    double result( 0.0 );

    if ( v1.Nrows() != v2.Nrows() )
    {
        throw std::range_error( "Vectors in scalar product need same size!" );
    }

    for ( unsigned long i( 1 ); i < v1.Nrows() + 1; i++ )
    {
        result += v1( i ) * v2( i );
    }

    return result;
}

ColumnVector FMath::cprod( const ColumnVector& v1, const ColumnVector& v2 )
{
    ColumnVector result( 3 );

    if ( v1.Nrows() != 3 || v2.Nrows() != 3 )
        throw std::range_error( "Vectors in cross product both need size 3!" );

    result( 1 ) = v1( 2 ) * v2( 3 ) - v1( 3 ) * v2( 2 );
    result( 2 ) = v1( 3 ) * v2( 1 ) - v1( 1 ) * v2( 3 );
    result( 3 ) = v1( 1 ) * v2( 2 ) - v1( 2 ) * v2( 1 );

    result = result / sqrt( FMath::iprod( result, result ) );

    return result;
}


void FMath::evd3x3( ColumnVector tensor, QVector<ColumnVector>& vecs, ColumnVector& vals )
{
    double i1, i2, i3, v, s, phi, l1, l2, l3;
    double ev1_x, ev1_y, ev1_z, ev2_x, ev2_y, ev2_z, ev3_x, ev3_y, ev3_z, vec_norm;

    double xx = tensor( 1 );
    double xy = tensor( 2 );
    double xz = tensor( 3 );
    double yy = tensor( 4 );
    double yz = tensor( 5 );
    double zz = tensor( 6 );

    // three invariants of D (dt)
    // i1=l1+l2+l3 (trace)
    // i2=l1*l2+l1*l3+l2*l3
    // i3=l1*l2*l3 (determinante)
    i1 = xx + yy + zz;
    i2 = xx * yy + xx * zz + yy * zz - ( pow2( xy ) + pow2( xz ) + pow2( yz ) );
    i3 = xx * yy * zz + 2. * xy * xz * yz - ( zz * pow2( xy ) + yy * pow2( xz ) + xx * pow2( yz ) );

    v = pow2( i1 / 3 ) - i2 / 3;
    s = pow3( i1 / 3 ) - i1 * i2 / 6 + i3 / 2;

    if ( ( v > 0 ) && ( pow2( s ) < pow3( v ) ) )
        phi = acos( s / v * sqrt( 1. / v ) ) / 3;
    else
        phi = 0;

    // eigenvalues
    if ( phi != 0 )
    {
        l1 = i1 / 3 + 2 * sqrt( v ) * cos( phi );
        l2 = i1 / 3 - 2 * sqrt( v ) * cos( M_PI / 3. + phi );
        l3 = i1 / 3 - 2 * sqrt( v ) * cos( M_PI / 3. - phi );
    }
    else
        l1 = l2 = l3 = 0.0;

    // eigenvectors
    ev1_x = ( xy * yz - ( yy - l1 ) * xz ) * ( xz * yz - ( zz - l1 ) * xy );
    ev1_y = ( xz * yz - ( zz - l1 ) * xy ) * ( xz * xy - ( xx - l1 ) * yz );
    ev1_z = ( xy * yz - ( yy - l1 ) * xz ) * ( xz * xy - ( xx - l1 ) * yz );

    ev2_x = ( xy * yz - ( yy - l2 ) * xz ) * ( xz * yz - ( zz - l2 ) * xy );
    ev2_y = ( xz * yz - ( zz - l2 ) * xy ) * ( xz * xy - ( xx - l2 ) * yz );
    ev2_z = ( xy * yz - ( yy - l2 ) * xz ) * ( xz * xy - ( xx - l2 ) * yz );

    ev3_x = ( xy * yz - ( yy - l3 ) * xz ) * ( xz * yz - ( zz - l3 ) * xy );
    ev3_y = ( xz * yz - ( zz - l3 ) * xy ) * ( xz * xy - ( xx - l3 ) * yz );
    ev3_z = ( xy * yz - ( yy - l3 ) * xz ) * ( xz * xy - ( xx - l3 ) * yz );

    vec_norm = sqrt( pow2( ev1_x ) + pow2( ev1_y ) + pow2( ev1_z ) );

    if ( vec_norm > 0 )
    {
        ev1_x = ev1_x / vec_norm;
        ev1_y = ev1_y / vec_norm;
        ev1_z = ev1_z / vec_norm;
    }
    else
        ev1_x = ev1_y = ev1_z = 0.0;

    vec_norm = sqrt( pow2( ev2_x ) + pow2( ev2_y ) + pow2( ev2_z ) );

    if ( vec_norm > 0 )
    {
        ev2_x = ev2_x / vec_norm;
        ev2_y = ev2_y / vec_norm;
        ev2_z = ev2_z / vec_norm;
    }
    else
        ev2_x = ev2_y = ev2_z = 0.0;

    vec_norm = sqrt( pow2( ev3_x ) + pow2( ev3_y ) + pow2( ev3_z ) );

    if ( vec_norm > 0 )
    {
        ev3_x = ev3_x / vec_norm;
        ev3_y = ev3_y / vec_norm;
        ev3_z = ev3_z / vec_norm;
    }
    else
        ev3_x = ev3_y = ev3_z = 0.0;

    ColumnVector ev1( 3 );
    ColumnVector ev2( 3 );
    ColumnVector ev3( 3 );

    vals(1) = l1;
    vals(2) = l2;
    vals(3) = l3;

    ev1( 1 ) = ev1_x;
    ev1( 2 ) = ev1_y;
    ev1( 3 ) = ev1_z;
    ev2( 1 ) = ev2_x;
    ev2( 2 ) = ev2_y;
    ev2( 3 ) = ev2_z;
    ev3( 1 ) = ev3_x;
    ev3( 2 ) = ev3_y;
    ev3( 3 ) = ev1_z;
    vecs.push_back( ev1 );
    vecs.push_back( ev2 );
    vecs.push_back( ev3 );
}

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
ColumnVector FMath::cart2sphere( const ColumnVector& input )
{
  if ( input.Nrows() != 3 )
    throw std::range_error("Can only transform 3D-vectors.");

  ColumnVector result( 3 );

    result( 1 ) = sqrt( input( 1 ) * input( 1 ) + input( 2 ) * input( 2 ) + input( 3 ) * input( 3 ) );
    result( 2 ) = acos( input( 3 ) / result( 1 ) );
    result( 3 ) = atan2( input( 2 ), input( 1 ) );

  return result;
}

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
ColumnVector FMath::sphere2cart( const ColumnVector& input )
{
    if ( input.Nrows() != 3 )
        throw std::range_error( "Can only transform 3D-vectors." );

    ColumnVector result( 3 );

    result( 1 ) = input( 1 ) * sin( input( 2 ) ) * cos( input( 3 ) );
    result( 2 ) = input( 1 ) * sin( input( 2 ) ) * sin( input( 3 ) );
    result( 3 ) = input( 1 ) * cos( input( 2 ) );

    return result;
}

ColumnVector FMath::SH_opt_max( const ColumnVector& startX, const ColumnVector& coeff )
{
    ColumnVector newX( startX );
    ColumnVector oldX( 3 );

    // set values for computation
    const double precision( 10.e-6 );
    const double eps( 10.e-4 );

    double delta( 1.0 );
    int steps( 0 );
    while ( delta > precision && steps < 100 )
    {
        // Update old value
        steps++;
        oldX = newX;
        double SH_old( sh_eval( oldX, coeff ) );

        // define the delta steps
        ColumnVector dt( oldX );
        ColumnVector dp( oldX );
        dt( 2 ) += eps;
        dp( 3 ) += eps;

        // calculate Jacobian
        double Jt( ( FMath::sh_eval( dt, coeff ) - SH_old ) );
        double Jp( ( FMath::sh_eval( dp, coeff ) - SH_old ) );

        // do iteration
        newX( 2 ) = oldX( 2 ) + Jt;
        newX( 3 ) = oldX( 3 ) + Jp;

        delta = fabs( Jt ) + fabs( Jp );
        //std::cout<<delta<<" ";
    }
    return newX;
}

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
double FMath::sh_eval( const ColumnVector& position, const ColumnVector& coeff )
{
    const int max_order( ( -3 + static_cast<int>( sqrt( 8 * coeff.Nrows() ) + 1 ) ) / 2 );

    const double radius( position( 1 ) );
    if ( radius == 0.0 )
    {
        return 0.0;
    }

    // for easier readability
    const double theta( position( 1 ) );
    const double phi( position( 2 ) );

    double result( 0 );
    for ( int order( 0 ), j( 1 ); order <= max_order; order += 2 )
    {
        for ( int degree( -order ); degree <= order; degree++, ++j )
        {
            result += coeff( j ) * FMath::sh_base_function( order, degree, theta, phi );
        }
    }
    return result;
}

// calculate rotation matrix: http://en.wikipedia.org/wiki/Rotation_matrix
Matrix FMath::RotationMatrix( const double angle, const ColumnVector& axis )
{
    double s( sin( angle ) );
    double c( cos( angle ) );
    double d( 1. - c );

    Matrix R( 3, 3 );
    R( 1, 1 ) = c + axis( 1 ) * axis( 1 ) * d;
    R( 1, 2 ) = axis( 1 ) * axis( 2 ) * d - axis( 3 ) * s;
    R( 1, 3 ) = axis( 1 ) * axis( 3 ) * d + axis( 2 ) * s;
    R( 2, 1 ) = axis( 1 ) * axis( 2 ) * d + axis( 3 ) * s;
    R( 2, 2 ) = c + axis( 1 ) * axis( 1 ) * d;
    R( 2, 3 ) = axis( 2 ) * axis( 3 ) * d - axis( 1 ) * s;
    R( 3, 1 ) = axis( 1 ) * axis( 3 ) * d - axis( 2 ) * s;
    R( 3, 2 ) = axis( 2 ) * axis( 3 ) * d + axis( 1 ) * s;
    R( 3, 3 ) = c + axis( 3 ) * axis( 3 ) * d;

    return R;
}

void FMath::debugColumnVector3( const ColumnVector& v, QString name )
{
    if ( v.Nrows() != 3 )
    {
        qDebug() << name << "error not 3 elements in vector";
    }
    qDebug() << name << v( 1 ) << v( 2 ) << v( 3 );
}

Matrix FMath::pseudoInverse( const Matrix& A )
{
    /*
    Matrix U( A.Nrows(), A.Ncols() );
    DiagonalMatrix D( A.Ncols() );
    Matrix V( A.Ncols(), A.Ncols() );
    SVD( A, D, U, V );
    return ( V * ( D.t() * D ) * D.t() * U.t() );
    */

    return ( ( A.t() * A ).i() * A.t() );
}

QVector<Matrix>* FMath::fitTensors( QVector<ColumnVector>* data, QVector<float> b0Images, QVector<QVector3D> bvecs, QVector<float> bvals )
{
    int N = bvecs.size();

    Matrix B( N, 6 );
    Matrix U( N, 6 );
    Matrix V( 6, 6 );
    Matrix BI( 6, N );
    DiagonalMatrix D( 6 );

    double mult_c;

    for ( int i = 0; i < N; ++i )
    {
        mult_c = bvals[i] / (float) ( bvecs[i].x() * bvecs[i].x() + bvecs[i].y() * bvecs[i].y() + bvecs[i].z() * bvecs[i].z() );

        B( i + 1, 1 ) = mult_c * bvecs[i].x() * bvecs[i].x();
        B( i + 1, 2 ) = 2 * mult_c * bvecs[i].x() * bvecs[i].y();
        B( i + 1, 3 ) = 2 * mult_c * bvecs[i].x() * bvecs[i].z();
        B( i + 1, 4 ) = mult_c * bvecs[i].y() * bvecs[i].y();
        B( i + 1, 5 ) = 2 * mult_c * bvecs[i].y() * bvecs[i].z();
        B( i + 1, 6 ) = mult_c * bvecs[i].z() * bvecs[i].z();
    }

    SVD( B, D, U, V );

    for ( int j = 1; j <= 6; ++j )
    {
        D( j ) = 1. / D( j );
    }
    BI = V * D * U.t();

    double s0 = 0.0;
    double si = 0.0;
    vector<double> log_s0_si_pixel( N );

    QVector<Matrix>* tensors = new QVector<Matrix>();

    Matrix blank( 3, 3 );
    blank = 0.0;

    for ( int i = 0; i < data->size(); ++i )
    {
        s0 = b0Images[i];

        if ( s0 > 0 )
        {
            // compute log(s0)-log(si)
            s0 = log( s0 );
            for ( int j = 0; j < N; ++j )
            {
                si = data->at( i )( j + 1 ); //dti[j*blockSize+i];
                if ( si > 0 )
                {
                    si = log( si );
                }
                else
                {
                    si = 0.0;
                }
                log_s0_si_pixel[j] = s0 - si;
            }

            double value;
            // compute tensor
            ColumnVector t( 6 );
            for ( int l = 0; l < 6; l++ )
            {
                value = 0;
                for ( int m = 1; m <= N; ++m )
                {
                    value += BI( l + 1, m ) * log_s0_si_pixel[m - 1];
                }
                t( l + 1 ) = (float) ( value ); // save the tensor components in a adjacent memory
            }
            Matrix m( 3, 3 );
            m( 1, 1 ) = t( 1 );
            m( 1, 2 ) = t( 2 );
            m( 1, 3 ) = t( 3 );
            m( 2, 1 ) = t( 2 );
            m( 2, 2 ) = t( 4 );
            m( 2, 3 ) = t( 5 );
            m( 3, 1 ) = t( 3 );
            m( 3, 2 ) = t( 5 );
            m( 3, 3 ) = t( 6 );

            tensors->push_back( m );
        } // end if s0 > 0
        else
        {
            tensors->push_back( blank );
        }
    }

    return tensors;
}

QVector<float> FMath::fa( QVector<Matrix>* tensors )
{
    int blockSize = tensors->size();

    QVector<float> trace( blockSize );
    float value = 0;
    for ( int i = 0; i < blockSize; ++i )
    {
        value = tensors->at( i )( 1, 1 );
        value += tensors->at( i )( 2, 2 );
        value += tensors->at( i )( 3, 3 );
        trace[i] = value / 3.0;
    }

    QVector<float> fa( blockSize );

    double xx, xy, xz, yy, yz, zz, tr, AA, DD;

    for ( int i = 0; i < blockSize; ++i )
    {
        xx = tensors->at( i )( 1, 1 );
        xy = tensors->at( i )( 1, 2 );
        xz = tensors->at( i )( 1, 3 );
        yy = tensors->at( i )( 2, 2 );
        yz = tensors->at( i )( 2, 3 );
        zz = tensors->at( i )( 3, 3 );
        tr = trace[i];

        AA = pow2( xx - tr ) + pow2( yy - tr ) + pow2( zz - tr ) + 2 * pow2( xy ) + 2 * pow2( xz ) + 2 * pow2( yz );
        DD = pow2( xx ) + pow2( yy ) + pow2( zz ) + 2 * pow2( xy ) + 2 * pow2( xz ) + 2 * pow2( yz );

        if ( DD > 0 )
        {
            fa[i] = (float) ( sqrt( AA ) / sqrt( DD ) * sqrt( 1.5 ) );
        }
        else
        {
            fa[i] = 0.0;
        }
    }

    return fa;
}

void FMath::evecs( QVector<Matrix>* tensors, QVector<QVector3D>& evec1, QVector<float>& eval1,
                                               QVector<QVector3D>& evec2, QVector<float>& eval2,
                                               QVector<QVector3D>& evec3, QVector<float>& eval3 )
{
    int blockSize = tensors->size();

    double xx, xy, xz, yy, yz, zz;
    double i1, i2, i3, v, s, phi, l1, l2, l3;
    double vec_norm, ev1_x, ev1_y, ev1_z, ev2_x, ev2_y, ev2_z, ev3_x, ev3_y, ev3_z;

    for ( int i = 0; i < blockSize; ++i )
    {
        xx = tensors->at( i )( 1, 1 );
        xy = tensors->at( i )( 1, 2 );
        xz = tensors->at( i )( 1, 3 );
        yy = tensors->at( i )( 2, 2 );
        yz = tensors->at( i )( 2, 3 );
        zz = tensors->at( i )( 3, 3 );

        // three invariants of D (dt)
        // i1=l1+l2+l3 (trace)
        // i2=l1*l2+l1*l3+l2*l3
        // i3=l1*l2*l3 (determinante)
        i1 = xx + yy + zz;
        i2 = xx * yy + xx * zz + yy * zz - ( pow2( xy ) + pow2( xz ) + pow2( yz ) );
        i3 = xx * yy * zz + 2. * xy * xz * yz - ( zz * pow2( xy ) + yy * pow2( xz ) + xx * pow2( yz ) );

        v = pow2( i1 / 3 ) - i2 / 3;
        s = pow3( i1 / 3 ) - i1 * i2 / 6 + i3 / 2;
        if ( ( v > 0 ) && ( pow2( s ) < pow3( v ) ) )
            phi = acos( s / v * sqrt( 1. / v ) ) / 3;
        else
            phi = 0;

        // eigenvalues
        if ( phi != 0 )
        {
            l1 = i1 / 3 + 2 * sqrt( v ) * cos( phi );
            l2 = i1 / 3 - 2 * sqrt( v ) * cos( M_PI / 3. + phi );
            l3 = i1 / 3 - 2 * sqrt( v ) * cos( M_PI / 3. - phi );
        }
        else
            l1 = l2 = l3 = 0.0;

        eval1[i] = l1;
        eval2[i] = l2;
        eval3[i] = l3;

        // eigenvectors
        ev1_x = ( xy * yz - ( yy - l1 ) * xz ) * ( xz * yz - ( zz - l1 ) * xy );
        ev1_y = ( xz * yz - ( zz - l1 ) * xy ) * ( xz * xy - ( xx - l1 ) * yz );
        ev1_z = ( xy * yz - ( yy - l1 ) * xz ) * ( xz * xy - ( xx - l1 ) * yz );

        ev2_x = ( xy * yz - ( yy - l2 ) * xz ) * ( xz * yz - ( zz - l2 ) * xy );
        ev2_y = ( xz * yz - ( zz - l2 ) * xy ) * ( xz * xy - ( xx - l2 ) * yz );
        ev2_z = ( xy * yz - ( yy - l2 ) * xz ) * ( xz * xy - ( xx - l2 ) * yz );

        ev3_x = ( xy * yz - ( yy - l3 ) * xz ) * ( xz * yz - ( zz - l3 ) * xy );
        ev3_y = ( xz * yz - ( zz - l3 ) * xy ) * ( xz * xy - ( xx - l3 ) * yz );
        ev3_z = ( xy * yz - ( yy - l3 ) * xz ) * ( xz * xy - ( xx - l3 ) * yz );

        vec_norm = sqrt( pow2( ev1_x ) + pow2( ev1_y ) + pow2( ev1_z ) );

        if ( vec_norm > 0 )
        {
            ev1_x = ev1_x / vec_norm;
            ev1_y = ev1_y / vec_norm;
            ev1_z = ev1_z / vec_norm;
        }
        else
            ev1_x = ev1_y = ev1_z = 0.0;

        vec_norm = sqrt( pow2( ev2_x ) + pow2( ev2_y ) + pow2( ev2_z ) );

        if ( vec_norm > 0 )
        {
            ev2_x = ev2_x / vec_norm;
            ev2_y = ev2_y / vec_norm;
            ev2_z = ev2_z / vec_norm;
        }
        else
            ev2_x = ev2_y = ev2_z = 0.0;

        vec_norm = sqrt( pow2( ev3_x ) + pow2( ev3_y ) + pow2( ev3_z ) );

        if ( vec_norm > 0 )
        {
            ev3_x = ev3_x / vec_norm;
            ev3_y = ev3_y / vec_norm;
            ev3_z = ev3_z / vec_norm;
        }
        else
            ev3_x = ev3_y = ev3_z = 0.0;

        evec1[i].setX( ev1_x );
        evec1[i].setY( ev1_y );
        evec1[i].setZ( ev1_z );
        evec2[i].setX( ev2_x );
        evec2[i].setY( ev2_y );
        evec2[i].setZ( ev2_z );
        evec3[i].setX( ev3_x );
        evec3[i].setY( ev3_y );
        evec3[i].setZ( ev3_z );
    }
}
