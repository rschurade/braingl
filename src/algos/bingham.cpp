/*
 * bingham.cpp
 *
 *  Created on: Nov 23, 2012
 *      Author: schurade
 */

#include "bingham.h"

Bingham::Bingham()
{
}

Bingham::~Bingham()
{
}
/*
void Bingham::calc_bingham(
  const Image& sh,
  const unsigned long depth,
  const unsigned long neighbourhood,
  const unsigned long num_max,
  Image& bingham )
{
  const unsigned long X(bingham.get_x()), Y(bingham.get_y()), Z(bingham.get_z());

  // Set tesselation and adjacency necessary for calculation:
  std::shared_ptr<Matrix> tesselation( tess::vertices( depth ) );
  std::shared_ptr<Matrix> adjacency( tess::adjacency( depth ) );

  // Calculate SH base:
  const int order((-3+static_cast<int>(sqrt(8*((*sh.get(0,0,0)).size())+1)))/2);
  std::shared_ptr<Matrix> base( sh_base( *tesselation, order ) );

  // For all voxels:
  #pragma omp parallel for
  for( unsigned long x=0; x < X; x++ )
    for( unsigned long y=0; y < Y; y++ )
      for( unsigned long z=0; z < Z; z++ )
        bingham(x,y,z) = fit_bingham( *sh.get(x,y,z), *tesselation, *adjacency,
                                     *base, neighbourhood, num_max );
}

std::shared_ptr<Vector> Bingham::fit_bingham(
  const Vector& sh_data,
  const Matrix& tess,
  const Matrix& adj,
  const Matrix& base,
  const unsigned long neighborhood,
  const unsigned long num_max )
{

  unsigned int mod = 14;

  // reserve memory:
  std::shared_ptr<Vector> result( new Vector(num_max*mod,true) );

  // if no CSD no fit necessary.
  if( sh_data(0) == 0 ) return result;

  // get maxima:
  Vector radius( prod( base, sh_data ) );
  std::shared_ptr< std::vector<unsigned long> >
    max( ranked_maxima_index( radius, adj, num_max ));

  // For all maxima:
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
      // for positive direction:
      for( unsigned long j(g.size()); gi < j; gi++ )
        for( unsigned long k(0); k < 6 && adj( g[gi], k ) > 0; k++ )
          if( !index[ adj( g[gi], k ) ] )
          {
            if( radius( g[gi] ) > radius( adj( g[gi], k ) ) && radius( adj( g[gi], k ) ) > 0.0 )
              g.push_back( adj( g[gi], k ) );

            index[ adj( g[gi], k ) ] = 1;
          }
      // for opposite direction:
      for( unsigned long j(h.size()); hi < j; hi++ )
        for( unsigned long k(0); k < 6 && adj( h[hi], k ) > 0; k++ )
          if( !index[ adj( h[hi], k ) ] )
          {
            if( radius( h[hi] ) > radius( adj( h[hi], k ) ) && radius( adj( h[hi], k ) ) > 0.0 )
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
    maxV(0) = tess( g[0], 0 );
    maxV(1) = tess( g[0], 1 );
    maxV(2) = tess( g[0], 2 );

    double f0 = radius( g[0] );

    for( unsigned long i(0), j(0); i < g.size(); i++ )
    {
      Vector cur(3);
      cur(0) = tess( g[i], 0 );
      cur(1) = tess( g[i], 1 );
      cur(2) = tess( g[i], 2 );

      double temp = radius( g[i] );

      if( temp > 0.0 && temp <= f0 )
      {
        if( iprod( cur, maxV ) < 0 ) cur = cur * (-1);
        points.push_back(cur);

        cur = *cart2sphere(cur);
        values(j++) = temp;
      }
    }

    // calculate moment_of_inertia and extract values:
    std::shared_ptr<Vector>  m_inertia( moment_of_inertia(values,points) );
    std::vector<Vector> vecs; Vector vals(3);

    // the eigenvectors are the bingham parameter mu:
    evd3x3( *m_inertia, vals, vecs );

    maxV = *sphere2cart( SH_opt_max( *cart2sphere(vecs[0]), sh_data ) );

    double angle( acos( iprod( maxV, vecs[0] ) ) );

    if( angle > .1 * M_PI/180. )
    {
      Vector ax( cprod( maxV, vecs[0] ) ); //axis

      Matrix R( *inverse( RotationMatrix( angle, ax ) ) );

      vecs[0] = maxV;
      vecs[1] = prod( R, vecs[1] );
      vecs[2] = prod( R, vecs[2] );

    }
    // copies of these vectors in spherical coordinates:
    Vector z0(*cart2sphere(vecs[0]));
    Vector z1(*cart2sphere(vecs[1]));
    Vector z2(*cart2sphere(vecs[2]));

    // preprocessing for moment of inertia matrix:
    //Vector values( g.size()+2, true );
    //std::vector<Vector> points;
    //Vector maxV(3);
    //maxV(0) = tess(g[0], 0); maxV(1) = tess(g[0], 1); maxV(2) = tess(g[0], 2);


    //std::cout <<"before: "<< maxV <<" "<< sh_eval( *cart2sphere(maxV), sh_data )<<std::endl;
    //Vector vecs[0]( *sphere2cart( SH_opt_max( *cart2sphere(maxV), sh_data ) ) );
    //Vector vecs[1]( SH_opt_mu( vecs[0], 2, sh_data ) );
    //Vector vecs[2](3);
    //vecs[2](0)=vecs[0](1)*vecs[1](2)-vecs[0](2)*vecs[1](1);
    //vecs[2](1)=vecs[0](2)*vecs[1](0)-vecs[0](0)*vecs[1](2);
    //vecs[2](2)=vecs[0](0)*vecs[1](1)-vecs[0](1)*vecs[1](0);



    // copies of these vectors in spherical coordinates:
    //Vector z0(*cart2sphere(vecs[0]));
    //Vector z1(*cart2sphere(vecs[1]));
    //Vector z2(*cart2sphere(vecs[2]));

    // get function value at maximum:
    f0 = sh_eval( z0, sh_data );

    if( g.size() > 2 )
    {
      // reserve calculation variables:
      Matrix A_tmp(g.size(),2,true);
      Vector b_tmp(g.size(),true);
      Vector index(g.size(),true);

      unsigned long size(0);
      // build matrix for least square solution:
      for( unsigned long i(0); i < g.size(); i++ )
      {
        Vector cur(3);
        cur(0) = tess(g[i],0); cur(1) = tess(g[i],1); cur(2) = tess(g[i],2);

        double f( radius(g[i]) );
        if( f0 > f && f > 0.0 )
        {
          A_tmp(i,0) = -iprod(vecs[1],cur)*iprod(vecs[1],cur);
          A_tmp(i,1) = -iprod(vecs[2],cur)*iprod(vecs[2],cur);
          b_tmp(i) = log(f/f0);
          size ++;
          index(i) = 1.0;
        }
      }

      Matrix A(size,2,true);
      Vector b(size,true);

      if( size != g.size() )
      {
        size = 0;
        for( unsigned long i(0); i < g.size(); i++ )
        {
          if( index(i) != 0.0 )
          {
            A(size,0) = A_tmp(i,0);
            A(size,1) = A_tmp(i,1);
            b(size) = b_tmp(i);
            size++;
          }
        }
      }
      else
      {
        A = A_tmp;
        b = b_tmp;
      }

      if ( size > 2 )
      {
        Vector k_s( prod( *pseudoinverse( A ), b ) );
        if( k_s(0) > 0.0 && k_s(1) > 0.0 )
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

          (*result)(n_max*mod+12) = 0.0;//maxI;
          (*result)(n_max*mod+13) = 0.0;//angle;
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
  }

  return result;
}
*/
