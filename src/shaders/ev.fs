#version 330

#include peel_fs

in float v_discard;
in vec4 frontColor;

void main( void )
{
    if ( v_discard > 0. )
    {
        discard;
    }

    writePeel( frontColor.rgb );
}
