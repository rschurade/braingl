#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include lighting_fs
#include peel_fs

in float v_discard;
uniform float u_fibGrowth;

void main()
{
    if ( v_index > u_fibGrowth )
    {
        discard;
    }

    vec3 color = frontColor.rgb;
    if ( v_discard > 0.0 )
    {
        discard;
    }
    if ( u_colorMode == 3 )
    {
        vec4 tcolor = texColor();
	    
	    if ( !( tcolor.r + tcolor.g + tcolor.b > 0.0 ) ) discard;
	    
	    color = tcolor.rgb;
    }
    else if ( u_colorMode == 4 )
    {
        if ( v_extra > u_upperThreshold || v_extra < u_lowerThreshold )
        {
            discard;
        }
        color = colormap( v_extra, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax ).rgb;
    }
    else
    {
        color = frontColor.rgb;
    }
    
    writePeel( light( vec4( color, 1.0 ) ).rgb );
}
