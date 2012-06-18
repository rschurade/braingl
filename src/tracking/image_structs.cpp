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
#include "image_structs.hpp"

#include <omp.h>

Image::Image()
{
  // initialize with zero:
  x_ = y_ = z_ = 0;
}



Image::Image( const char* file )
{
  // read the image:
  nifti_image *input( nifti_image_read( file, long(1) ) );
  if( !input ) 
    throw std::ios_base::failure( "Failed to read NIfTI image!" );
  
  // read the header object:
  header_ = nifti_convert_nim2nhdr( input );
  
  x_ = header_.dim[1];  y_ = header_.dim[2];  z_ = header_.dim[3];        
        
  switch (input->datatype)
  {
    case DT_SIGNED_SHORT: read_nii<signed short> ( *input, data_ ); break;
    case DT_BINARY: read_nii<bool>( *input, data_ ); break;
    case DT_UNSIGNED_CHAR: read_nii<unsigned char>( *input, data_ ); break;  
    case DT_SIGNED_INT: read_nii<signed int>( *input, data_ ); break;
    case DT_FLOAT: read_nii<float>( *input, data_ ); break;
    case DT_DOUBLE: read_nii<double>( *input, data_ ); break;   
    default: throw std::ios_base::failure( "Datatype not supported" ); break;
  }
  
  nifti_image_free( input ); 
}



Image::Image( const nifti_1_header header, 
       const unsigned long x, 
       const unsigned long y, 
       const unsigned long z )
{
  header_ = header;
  x_ = x;
  y_ = y;
  z_ = z;
  data_.resize( x_ * y_ * z_ );
}

Image::Image( const nifti_1_header header, 
       const unsigned long x, 
       const unsigned long y, 
       const unsigned long z, 
       const std::vector< boost::shared_ptr<Vector> >& data )
{
  header_ = header;
  x_ = x;
  y_ = y;
  z_ = z;
  data_ = data;
}

Image::Image( const Image& input )
{
  header_ = input.get_header();
  
  x_ = input.get_x();
  y_ = input.get_y();
  z_ = input.get_z();
  
  data_.resize( x_ * y_ * z_ );
}


void Image::write( const char *filename, nifti_type outtype )     
{   
  short int original_type( header_.datatype );
  header_.datatype = outtype;
  
  switch( outtype )
  {
    case INT: write_nii< signed int >( filename ); break;
    case CHAR: write_nii< unsigned char >( filename ); break;
    case FLOAT: write_nii< float >( filename ); break;
    case DOUBLE: write_nii< double >( filename ); break;
    case SHORT: write_nii< signed short >( filename ); break;
    default: throw std::ios_base::failure( "Datatype not supported" ); break;
  }      
  
  header_.datatype = original_type; //restore orginal header
}

template < typename TYPE > 
void  Image::read_nii( const nifti_image& input,
                   std::vector< boost::shared_ptr<Vector> >& output )
{
  // read data:
  TYPE* data = (TYPE*) input.data;
  output.reserve( x_ * y_ * z_ );
  
  // for all voxels:
  for( unsigned long z=0; z < z_; z++ )
    for( unsigned long y(0); y < y_; y++ )
      for( unsigned long x(0); x < x_; x++ )
      {
        boost::shared_ptr<Vector> voxel( new Vector(input.nt) );
        
        for( int t=0; t < input.nt; t++ )
        {
          (*voxel)(t) = static_cast<double>(data[x+y*x_+z*x_*y_+t*x_*y_*z_]);     
        }
        output.push_back( voxel );
      }
}

template < typename TYPE >  
void Image::write_nii( const char *filename ) 
{ 
  // if any voxel has more than one data point set fourth dimension:
  unsigned long max_dim(0);
  for( unsigned long x(0); x < x_; x++ )
    for( unsigned long y(0); y < y_; y++ )
      for( unsigned long z(0); z < z_; z++ )
      {
        max_dim = ( max_dim > (*(*this)(x,y,z)).size() )
                ?   max_dim : (*(*this)(x,y,z)).size();
      }
 
  header_.dim[1] = x_;
  header_.dim[2] = y_;
  header_.dim[3] = z_; 
 
  // if image image has only one dimension:
  if( max_dim == 1 )
  {
    header_.dim[4] = 1;
    header_.dim[0] = 3;
  }
  else
  {       
    header_.dim[4] = max_dim;
    header_.dim[0] = 4;
  }
          
  // search for biggest value in image and for nan or infinite values:
  TYPE max(0);
  for( unsigned long x(0); x < x_; x++ )
    for( unsigned long y(0); y < y_; y++ )
      for( unsigned long z(0); z < z_; z++ )
        for( unsigned long t(0); t < (*(*this)(x,y,z)).size(); t++ )
        {
          // set infinite or nan to 0
          if( !finite( (*(*this)(x,y,z))(t) ) ) (*(*this)(x,y,z))(t) = 0;
          max = (max > (*(*this)(x,y,z))(t))? max :(*(*this)(x,y,z))(t);
        }
  header_.cal_max = max; // store maximum in header
     
  // copy data in right order for nifti image
  std::vector< TYPE > output( x_ * y_ * z_ * max_dim );
 
  // for int,char,short you have to round!
  const short int datatype( header_.datatype );
  if ( datatype == CHAR || datatype == SHORT || datatype == INT )
  {
    #pragma omp parallel for
    for( unsigned long x=0; x < x_; x++ )
      for( unsigned long y(0); y < y_; y++ )
        for( unsigned long z(0); z < z_; z++ )
        {
          for( unsigned long t(0); t < (*(*this)(x,y,z)).size(); t++ )
            output[x+y*x_+z*x_*y_+t*x_*y_*z_] = round( (*(*this)(x,y,z))(t) );
          for( unsigned long t((*(*this)(x,y,z)).size()); t < max_dim; t++ )
            output[x+y*x_+z*x_*y_+t*x_*y_*z_] = 0;
          
          // free the variable after writing it to file.
          (*this)(x,y,z).reset();
        } 
  }
  else
  {
    #pragma omp parallel for
    for( unsigned long x=0; x < x_; x++ )
      for( unsigned long y(0); y < y_; y++ )
        for( unsigned long z(0); z < z_; z++ )
        {
          for( unsigned long t(0); t < (*(*this)(x,y,z)).size(); t++ )
            output[x+y*x_+z*x_*y_+t*x_*y_*z_] = (*(*this)(x,y,z))(t);
          for( unsigned long t((*(*this)(x,y,z)).size()); t < max_dim; t++ )
            output[x+y*x_+z*x_*y_+t*x_*y_*z_] = 0;
          
          // free the variable after writing it to file.
          (*this)(x,y,z).reset();
        }
  }
      
  // generate output nifti file:
  nifti_image* output_image( nifti_convert_nhdr2nim( header_, filename ) );  
  output_image->data = &output[0];
            
  // write file & free memory:
  nifti_image_write( output_image );
  output_image->data = NULL;
  nifti_image_free( output_image );    
}


//This frees the memory reserved for the raw data:
void Image::extract_b_zero( const Vector& g_mask, Image& data, Image& b_zero )
{
  unsigned long quot(0);
  for( unsigned long t(0); t < g_mask.size(); t++ )
    if( g_mask(t) <= 0 ) quot++;
    
  //#pragma omp parallel for
  for( unsigned long x=0; x < x_; x++ )
    for( unsigned long y(0); y < y_; y++ )
      for( unsigned long z(0); z < z_; z++ )
      {
        boost::shared_ptr<Vector> voxel_b_zero( new Vector(1) );
        boost::shared_ptr<Vector> voxel_data( new Vector(g_mask.size()-quot) );
        double sum(0.0);
        unsigned long u(0);
        for( unsigned long t(0); t < g_mask.size(); t++ )
        {
          if( g_mask(t) <= 0 )
          {
            sum += (*data_[ x + y*x_ + z*x_*y_ ])(t);
          }
          else
          {
            (*voxel_data)(u++) = (*data_[ x + y*x_ + z*x_*y_ ])(t);
          }
        }
        (*voxel_b_zero)(0) = sum / (double)quot;
        b_zero( x, y, z ) = voxel_b_zero;
        data(x,y,z)=voxel_data;
       
        (data_[ x + y*x_ + z*x_*y_ ]).reset();
      }
}
