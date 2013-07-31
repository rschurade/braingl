#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include peel_fs

in float v_discard;

void main()
{
    vec3 color = frontColor.rgb;
    if ( v_discard > 0.0 )
    {
        discard;
    }
    if ( u_colorMode == 3 )
    {
        vec4 tcolor = texColor( v_texcoord );
	    
	    if ( !( tcolor.r + tcolor.g + tcolor.b > 0.0 ) ) discard;
	    
	    color = tcolor.rgb;
    }
    else if ( u_colorMode == 4 )
    {
        color = colormap( v_extra, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax ).rgb;
    }
    else
    {
        color = frontColor.rgb;
    }
    
    writePeel( color );
}
