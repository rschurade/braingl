#version 330 

#include uniforms_fs
#include peel_fs

void main( void )
{
    vec3 color = u_color.rgb;
    
    writePeel( color );
}
