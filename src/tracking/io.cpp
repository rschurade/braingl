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

#include "io.hpp"
#include "math.hpp"
#include "tesselation.hpp"

#include <iostream>
#include <fstream>

///* ***************************************************************************
///* @file io.cpp
///* @fn matrixT read_gradients(
///* 
///*     const char *filename,
///*     const sizeT number )
///* 
///* @brief TODO
///* 
///* @param TODO
///* 
///* @return TODO
///* ***************************************************************************
Matrix read_gradients( const char *file_bval, const char *file_bvec, 
                       Vector& b_values )
{
  // open the b-value file:  
  FILE *fp1= fopen(file_bval,"r");
  if (!fp1) throw std::ios_base::failure("Failed to open b-value file!");
  
  // copy datay:
  std::vector<double> b_val;
  double tmp;
  while( feof(fp1)==0 ) if( fscanf(fp1,"%lf",&tmp)==1 ) b_val.push_back(tmp);
  
  // close file:
  fclose(fp1);
  
  // build vector from data:
  unsigned long size(0);
  b_values =  Vector( b_val.size() );
  for ( unsigned long i(0); i < b_val.size(); i++ )
  {
    if ( b_val[i] > 0 ) size++;
    b_values(i) = b_val[i];
  }

  // open gradient file:
  FILE *fp2= fopen(file_bvec,"r");
  if (!fp2) throw std::ios_base::failure("Failed to open b-vector file!");
  
  // read amd copy data:
  Matrix gradients(size,3);
  for ( unsigned long j(0); j < 3; j++ )
  {
    for ( unsigned long i(0), k(0); i <  b_values.size(); i++ )
    {
      if ( feof(fp2) != 0 )
        throw std::ios_base::failure("Gradient file does not match data!");
      else fscanf( fp2, "%lf", &tmp );
      if( b_values(i) > 0 ) gradients(k++,j) = tmp;
    }
  }
  
  // close file:
  fclose(fp2);
  
  return gradients;
}

///* ***************************************************************************
///* @file io.cpp
///* @fn vector read_kernel(
///* 
///*     const char *filename,
///*     const sizeT order )
///* 
///* @brief TODO
///* 
///* @param TODO
///* 
///* @return TODO
///* ***************************************************************************
std::vector<double> read_kernel( const char *file )
{
  // open the b-value file:  
  FILE *fp= fopen(file,"r");
  if (!fp) throw std::ios_base::failure("Failed to open kernel file!");

  // copy datay:
  std::vector<double> result;
  double tmp;
  while( feof(fp)==0 ) if( fscanf(fp,"%lf",&tmp)==1 ) result.push_back(tmp);
  
  // close file:
  fclose(fp);
  
  return result;
}

///* ***************************************************************************
///* @file io.cpp
///* @fn vector read_kernel(
///* 
///*     const char *filename,
///*     const sizeT order )
///* 
///* @brief TODO          Matrix::Matrix(gsl_matrix*)      
///* 
///* @param TODO
///* 
///* @return TODO
///* ***************************************************************************
std::vector<Vector> read_voxels( const char *file )
{
  // open the b-value file:  
  FILE *fp= fopen(file,"r");
  if (!fp) throw std::ios_base::failure("Failed to open voxel file!");
  
  // copy datay:
  std::vector<Vector> result;
  Vector tmp(3);
  while( feof(fp)==0 ) if( fscanf(fp,"%lf %lf %lf\n",&tmp(0),&tmp(1),&tmp(2))==3 ) 
  {
    result.push_back(tmp);
  }
    
  
  // close file:
  fclose(fp);
  
  return result;
}

///* ***************************************************************************
///* @file io.cpp
///* @fn vector write_kernel(
///* 
///*    const std::vector<double>& kernel
///*    const char *filename )
///*   
///* @brief TODO
///* 
///* @param TODO
///* 
///* @return TODO
///* ***************************************************************************
void write_kernel( 
  const std::vector<double>& response,
  const char *filename )
{
  std::ofstream response_file (filename, std::ios::out);

  for (unsigned long i(0); i < response.size(); i++) 
  {
    response_file << response[i] << " ";
  }
  response_file << "\n";
}

///* ***************************************************************************
///* @file io.cpp
///* @fn vector read_kernel(
///* 
///*     const Matrix,
///*     const Matrix )
///* 
///* @brief TODO
///* 
///* @param TODO
///* 
///* @return TODO
///* ***************************************************************************
Matrix orient_gradients( const Matrix& gradients, const Matrix& sform )
{
  // Reserve output object:
  
  if( sform(0,0)*sform(0,0)+sform(1,0)*sform(1,0)+sform(2,0)*sform(2,0)
    + sform(0,1)*sform(0,1)+sform(1,1)*sform(1,1)+sform(2,1)*sform(2,1)
    + sform(0,2)*sform(0,2)+sform(1,2)*sform(1,2)+sform(2,2)*sform(2,2) > 0.0)
  {
    Matrix result( gradients.size1(), gradients.size2() );

    // Update the gradient values using the sform matrix and ensure unit length: 
    for( unsigned int i(0); i < gradients.size1(); ++i )
    {
      Vector tmp(3);
      tmp(0) = gradients(i,0); tmp(1) = gradients(i,1); tmp(2) = gradients(i,2);
      tmp = prod( sform, tmp );
      tmp = tmp / tmp.norm();

      result(i,0) = tmp(0); result(i,1) = tmp(1); result(i,2) = tmp(2);
    }
    return result;
  }
  else return gradients;
}

/* *************************************************************************//**
 * @file io.cpp
 * @fn void write_glyph(
 * 
 *     const char *filename,
 *     const imageT& my_image,
 *     const vectorT dist,
 *     const sizeT order,
 *     const sizeT steps,
 *     const rangeS view )
 * 
 * @brief Writes glyph information of a spherical harmonic file to a .dip file.
 * 
 * For further information view:                                            <br>
 *  TODO
 * 
 * @param filename
 * @param image from which glyph data is extracted
 * @param parameters
 * @param range
 * ****************************************************************************/
void write_sh_glyph(

  const char *filename,
  const Image& my_image,
  const unsigned int steps,
  const double color )
{       
  FILE *out_f = fopen( filename, "w" );
  if (!out_f) throw std::ios_base::failure( "Failed to write file!" );

  //get tesselation:
  boost::shared_ptr<Matrix> sphere_v( tess::vertices( steps ) );
  boost::shared_ptr<Matrix> sphere_f( tess::faces( steps ) );

  const int order((-3+(int)(sqrt(8*((*my_image.get(0,0,0)).size())+1)))/2);
  
  // calculate sh_base: (could be optimized via lookup table)
  boost::shared_ptr<Matrix> base( sh_base( *sphere_v, order ) );
  
  Vector dist(3);
  dist(0) = my_image.get_header().pixdim[1];
  dist(1) = my_image.get_header().pixdim[2];
  dist(2) = my_image.get_header().pixdim[3];
  
  // calculate size parameters:
  unsigned long num (0);
  
  // only plot sh with sh(0) != 0.0
  for( unsigned long x(0); x < my_image.get_x(); x++ )
    for( unsigned long y(0); y < my_image.get_y(); y++ )
      for( unsigned long z(0); z < my_image.get_z(); z++ )
        num += ((*my_image.get(x,y,z))(0)!=0.0); 
      
  // number of vertices to be saved in file:
  int num_v = static_cast<int>(num * (*sphere_v).size1() + 2);

  // write dip file header:
  fprintf(out_f,"UnitPosition mm\nNumberPositions= %d\nPositionsFixed\n",num_v);

  // compute glyph at every voxel:
  for( unsigned long x(0); x < my_image.get_x(); x++ )
    for( unsigned long y(0); y < my_image.get_y(); y++ )
      for( unsigned long z(0); z < my_image.get_z(); z++ )
        if( (*my_image.get(x,y,z))(0)!=0.0 )
        {
          // calculate all the radii in the voxel and scale them:
          Vector radius( prod( *base, *my_image.get(x,y,z) ) );
          radius = radius / 4.0;
                    
          for( unsigned long i(0); i < radius.size(); i++ )
          {
              // remove negative lobes:
              if( radius(i) < 0 ) radius(i) = 0.0;
              // write data to file:
              
              // flipping x->-x, y and z for fibernavigator output:
              fprintf(out_f, "%.4f %8.4f %8.4f\n",
               (-radius(i)*(*sphere_v)(i,0)+.5+ x )*dist(0),
               ( radius(i)*(*sphere_v)(i,1)+.5+(my_image.get_y()-1-y))*dist(1),
               ( radius(i)*(*sphere_v)(i,2)+.5+(my_image.get_z()-1-z))*dist(2) );
          }
        }
  fprintf(out_f, "%.4f %8.4f %8.4f\n", -100.,-100.,-100. );
  fprintf(out_f, "%.4f %8.4f %8.4f\n", -100.,-100.,-100. );

  // calculate number of polygons and write header:
  int num_f = static_cast<int>(num * (*sphere_f).size1());
  fprintf(out_f,"NumberPolygons= %d\nTypePolygons= 3\nPolygons\n",num_f);

  // write polygons to file:
  for( unsigned long i(0); i < num; i++ )
    for( unsigned long j(0); j < (*sphere_f).size1(); j++ )
    {
      fprintf( out_f, "%d %d %d\n",
        (int) ( (*sphere_f)(j,0) + i * (*sphere_v).size1() ),
        (int) ( (*sphere_f)(j,2) + i * (*sphere_v).size1() ),
        (int) ( (*sphere_f)(j,1) + i * (*sphere_v).size1() ) );
    }

  // set coloring by setting magnitudes:
  fprintf( out_f, "Magnitudes\n" );
  if( color < 0.0 )
    for( unsigned long i(0); i < num; i++ )
      for( unsigned long j(0); j < (*sphere_v).size1(); j++ )
      {
        fprintf( out_f, "%.8f\n", 
          mri_color((*sphere_v)(j,0),(*sphere_v)(j,1),(*sphere_v)(j,2)));
      }
  else
    for( unsigned long i(0); i < num; i++ )
      for( unsigned long j(0); j < (*sphere_v).size1(); j++ )
      {
        fprintf( out_f, "%.8f\n", color );
      }

  fprintf( out_f, "%.8f\n", 0. );
  fprintf( out_f, "%.8f\n", 1. );
  // close file after writing:
  fclose(out_f);
}
/* ****************************************************************************/


/* *************************************************************************//**
 * @file io.cpp
 * @fn void write_glyph(
 * 
 *     const char *filename,
 *     const imageT& my_image,
 *     const vectorT dist,
 *     const sizeT order,
 *     const sizeT steps,
 *     const rangeS view )
 * 
 * @brief Writes glyph information of a spherical harmonic file to a .dip file.
 * 
 * For further information view:                                            <br>
 *  TODO
 * 
 * @param filename
 * @param image from which glyph data is extracted
 * @param parameters
 * @param range
 * ****************************************************************************/
void write_bingham_glyph(

  const char *filename,
  const Image& f0,
  const Image& m1,
  const Image& m2,
  const Image& k1,
  const Image& k2,
  const unsigned int steps,
  const double color )
{       
  FILE *out_f = fopen( filename, "w" );
  if (!out_f) throw std::ios_base::failure( "Failed to write file!" );

  //get tesselation:
  boost::shared_ptr<Matrix> sphere_v( tess::vertices( steps ) );
  boost::shared_ptr<Matrix> sphere_f( tess::faces( steps ) );
  
  Vector dist(3);
  dist(0) = f0.get_header().pixdim[1];
  dist(1) = f0.get_header().pixdim[2];
  dist(2) = f0.get_header().pixdim[3];
  
  // calculate size parameters:
  unsigned long num (0);
  
  // only plot sh with sh(0) != 0.0
  for( unsigned long x(0); x < f0.get_x(); x++ )
    for( unsigned long y(0); y < f0.get_y(); y++ )
      for( unsigned long z(0); z < f0.get_z(); z++ )
        num += ((*f0.get(x,y,z))(0)!=0.0); 
      
  // number of vertices to be saved in file:
  int num_v = static_cast<int>(num * (*sphere_v).size1() + 2);
  
  // write dip file header:
  fprintf(out_f,"UnitPosition mm\nNumberPositions= %d\nPositionsFixed\n",num_v);

  // compute glyph at every voxel:
  for( unsigned long x(0); x < f0.get_x(); x++ )
    for( unsigned long y(0); y < f0.get_y(); y++ )
      for( unsigned long z(0); z < f0.get_z(); z++ )
        if( (*f0.get(x,y,z))(0)!=0.0 )                       
          for( unsigned long i(0); i < (*sphere_v).size1(); i++ )
          {
            Vector cur(3);
            cur(0) = (*sphere_v)(i,0);
            cur(1) = (*sphere_v)(i,1);
            cur(2) = (*sphere_v)(i,2);
            
            double val_1( iprod( (*m1.get(x,y,z)), cur ) );
            double val_2( iprod( (*m2.get(x,y,z)), cur ) );
            
            double radius(0.0);
            
            radius = (*f0.get(x,y,z))(0) * exp( - ( 
                (*k1.get(x,y,z))(0)*val_1*val_1 + 
                (*k2.get(x,y,z))(0)*val_2*val_2 ) );
            
            radius = radius / 4.0;
            // write data to file:
            // flipping x->-x, y and z for fibernavigator output:
            fprintf(out_f, "%.4f %8.4f %8.4f\n",
               (-radius*(*sphere_v)(i,0)+.5+ x )*dist(0),
               ( radius*(*sphere_v)(i,1)+.5+(f0.get_y()-1-y))*dist(1),
               ( radius*(*sphere_v)(i,2)+.5+(f0.get_z()-1-z))*dist(2) );
          }
  fprintf(out_f, "%.4f %8.4f %8.4f\n", -100.,-100.,-100. );
  fprintf(out_f, "%.4f %8.4f %8.4f\n", -100.,-100.,-100. );

  // calculate number of polygons and write header:
  int num_f = static_cast<int>(num * (*sphere_f).size1());
  fprintf(out_f,"NumberPolygons= %d\nTypePolygons= 3\nPolygons\n",num_f);

  // write polygons to file:
  for( unsigned long i(0); i < num; i++ )
    for( unsigned long j(0); j < (*sphere_f).size1(); j++ )
    {
      fprintf( out_f, "%d %d %d\n",
        (int) ( (*sphere_f)(j,0) + i * (*sphere_v).size1() ),
        (int) ( (*sphere_f)(j,2) + i * (*sphere_v).size1() ),
        (int) ( (*sphere_f)(j,1) + i * (*sphere_v).size1() ) );
    }

  // set coloring by setting magnitudes:
  fprintf( out_f, "Magnitudes\n" );
  if( color < 0.0 )
    for( unsigned long i(0); i < num; i++ )
      for( unsigned long j(0); j < (*sphere_v).size1(); j++ )
      {
        fprintf( out_f, "%.8f\n", 
          mri_color((*sphere_v)(j,0),(*sphere_v)(j,1),(*sphere_v)(j,2)));
      }
  else
    for( unsigned long i(0); i < num; i++ )
      for( unsigned long j(0); j < (*sphere_v).size1(); j++ )
      {
        fprintf( out_f, "%.8f\n", color );
      }
  fprintf( out_f, "%.8f\n", 0. );
  fprintf( out_f, "%.8f\n", 1. );
  // close file after writing:
  fclose(out_f);
}
/* ****************************************************************************/

/* *************************************************************************//**
 * @file Utils.cpp
 * @fn baseT mri_color(
 * 
 *     const baseT X,
 *     const baseT Y,
 *     const baseT Z )
 * 
 * @brief Calculates the color of a point based on its direction. 
 * 
 * Only needed for visulisation.                                            <br>
 * For further information view:                                            <br>
 *  Wikipedia: http://en.wikipedia.org/wiki/HSL_and_HSV
 * 
 * @param x, y, z - components of the direction
 * 
 * 
 * @return a color value between 0 and 1 based on 
 *         the calculation of hue in hsv format.
 * ****************************************************************************/
double mri_color(

    const double X,
    const double Y,
    const double Z )
{
    // absolut values needed for computation:
    const double x(fabs(X)), y(fabs(Y)), z(fabs(Z));
    
    // find maximum and minimum values:
    const double a( (x>y)?((x>z)?x:z):((y>z)?y:z) ), 
                 b( (x<y)?((x<z)?x:z):((y<z)?y:z) );
    
    // actual calculation:
    double result(0.0);
    if( a == b )result = 0.0;
    else if( a == x ) result = ( 1.0/6.0 ) * ( 0.0 + ( y-z )/( a - b ) );
    else if( a == y ) result = ( 1.0/6.0 ) * ( 2.0 + ( z-x )/( a - b ) );  
    else if( a == z ) result = ( 1.0/6.0 ) * ( 4.0 + ( x-y )/( a - b ) );
    
    // return result:
    if( result < 0 ) return result + 1.0;
    else return result;
}

/* ****************************************************************************/
