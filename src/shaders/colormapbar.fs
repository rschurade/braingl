#version 330

#include colormap_fs
#include uniforms_fs
#include peel_fs

void main( void )
{
    float value = 0.0;
    value = v_texcoord.x * ( u_upperThreshold - u_lowerThreshold ) + u_lowerThreshold;
    float value2 = ( value - u_min ) / ( u_max - u_min );
    value2 = max( 0.0, min( 1.0, value2 ) );
    
    writePeel( colormap( value2, u_colormap, 0.0, 1.0, 0.0, 1.0 ).rgb );
}
