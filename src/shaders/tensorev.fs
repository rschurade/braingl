#version 330

in vec4 frontColor;

#include peel_fs

in float v_discard;

void main( void )
{
    if ( v_discard > 0. )
    {
        discard;
    }

    writePeel( frontColor );
}
