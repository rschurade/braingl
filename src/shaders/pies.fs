#version 330

#include uniforms_fs
#include peel_fs

in float v_discard;
flat in vec4 pieColor;

void main()
{ 
    if ( v_discard > 0.0 )
    {
        discard;
    }
    writePeel( pieColor.rgb );
}
