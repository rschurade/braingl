#version 120 

uniform vec4 u_color;

#include peel_fs

varying float v_discard;

void main( void )
{
    if ( v_discard > 0. )
    {
        discard;
    }

    writePeel( gl_Color.rgb );
}
