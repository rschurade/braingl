#version 330

#include uniforms_fs
#include lighting_fs

#include peel_fs

void main( void )
{
    if ( u_lighting )
    {
        vec3 color = light( u_color ).rgb;
        writePeel( color );
    }
    else
    {
        writePeel( u_color.rgb );
    }
}