#version 120 

#include uniforms_fs
#include lighting_fs

void main( void )
{
    if ( u_picking )
    {
        gl_FragColor = u_color;
    }
    else
    {
        light( u_color );
    }
}
