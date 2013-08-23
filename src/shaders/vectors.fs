#version 330

#include uniforms_fs
#include peel_fs

in float v_discard;

void main()
{ 
    if ( v_discard > 0.0 )
    {
        discard;
    }
    writePeel( frontColor );
}
