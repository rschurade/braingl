#version 330

#include uniforms_fs
#include peel_fs
#include colormap_fs

in float v_discard;
flat in vec4 pieColor;
flat in float out_value;

void main()
{ 
    if ( v_discard > 0.0 )
    {
        discard;
    }
    vec4 outColor = pieColor;
    if ( u_colorMode == 1 )
    {
       outColor = colormap( out_value, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
    }
    writePeel( outColor );
}
