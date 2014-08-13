#version 330

#include uniforms_fs
#include lighting_fs

#include peel_fs

void main( void )
{
    if ( u_lighting )
    {
        vec3 color = light( u_color ).rgb;
        writePeel( vec4( color, u_alpha ) );
    }
    else
    {
        writePeel( vec4( u_color.rgb, u_alpha ) );
    }
}