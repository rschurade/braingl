/*
 * fmath.h
 *
 * Created on: Jul 17, 2012
 * @author Ralph Schurade
 */

#ifndef FMATH_H_
#define FMATH_H_

#include "../thirdparty/newmat10/newmat.h"

#include <QVector>
#include <QString>
#include <QVector>
#include <QVector3D>

class FMath
{
public:
    static ColumnVector vlog( ColumnVector v );

    static double legendre_p( int k );
    static double boostLegendre_p( int order, int arg1, double arg2 );

    static Matrix sh_base( Matrix g, int max_order );
    static double sh_base_function( int order, int degree, double theta, double phi );

    static SymmetricMatrix moment_of_inertia( const ColumnVector& values, const std::vector<ColumnVector>& points );
    static double iprod( const ColumnVector& v1, const ColumnVector& v2 );
    static ColumnVector cprod( const ColumnVector& v1, const ColumnVector& v2 );

    static void evd3x3( ColumnVector tensor, std::vector<ColumnVector>& vecs, ColumnVector& vals );
//    void evd3x3_2( const ColumnVector &d, ColumnVector &val, std::vector<ColumnVector> &vec );
    static void evd3x3_2( Matrix &A, ColumnVector &val, std::vector<ColumnVector> &vec );

    static ColumnVector cart2sphere( const ColumnVector& input );
    static ColumnVector sphere2cart( const ColumnVector& input );


    static ColumnVector SH_opt_max( const ColumnVector& startX, const ColumnVector& coeff );
    static double sh_eval( const ColumnVector& position, const ColumnVector& coeff );

    static Matrix RotationMatrix( const double angle, const ColumnVector& axis );

    static void debugColumnVector3( const ColumnVector& v, QString name = QString( "" ) );

    static Matrix pseudoInverse( const Matrix& A );

    static void fitTensors( std::vector<ColumnVector>& data, std::vector<float>& b0Images, std::vector<QVector3D>& bvecs, std::vector<float>& bvals, std::vector<Matrix>& out );

    static void fa( std::vector<Matrix>& tensors, std::vector<float>& faOut );
    static float fa( Matrix tensor );

    static void evec1( std::vector<Matrix>& tensors, std::vector<QVector3D>& evec1 );
    static void evecs( std::vector<Matrix>& tensors, std::vector<QVector3D>& evec1, std::vector<float>& eval1,
                                                 std::vector<QVector3D>& evec2, std::vector<float>& eval2,
                                                 std::vector<QVector3D>& evec3, std::vector<float>& eval3 );

    static bool linePlaneIntersection( QVector3D& contact, QVector3D ray, QVector3D rayOrigin, QVector3D normal, QVector3D coord);

    static QVariant interpolateQVariant( QVariant &lhs, QVariant &rhs, float div );


    static Matrix expT( Matrix& t );


    static double pow2( double v )
    {
        return v * v;
    };
    static double pow3( double v )
    {
        return v * v * v;
    };

    static int pow2roundup( int x )
    {
        if ( x < 0 )
            return 0;
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x + 1;
    }

private:
    FMath();
    virtual ~FMath();


};

#endif /* FMATH_H_ */
