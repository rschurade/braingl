#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include lighting_fs
#include peel_fs

in float v_discard;
uniform float u_fibGrowth;
uniform int u_mriSource;

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
        vec4 tcolor = vec4( 0.0 );
        vec4 color0 = vec4( 0.0, 0.0, 0.0, 0.0 );
	    vec4 color1 = vec4( 0.0, 0.0, 0.0, 0.0 );
	    vec4 color2 = vec4( 0.0, 0.0, 0.0, 0.0 );
	    vec4 color3 = vec4( 0.0, 0.0, 0.0, 0.0 );
	    vec4 color4 = vec4( 0.0, 0.0, 0.0, 0.0 );
        if ( u_mriSource == 0 )
        {
            color0 = texture( texture0, v_texCoord0 );
            tcolor = colormap( unpackFloat( color0 ), u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
        }
        if ( u_mriSource == 1 )
        {
            color1 = texture( texture1, v_texCoord1 );
            tcolor = colormap( unpackFloat( color1 ), u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
        }
        if ( u_mriSource == 2 )
        {
            color2 = texture( texture2, v_texCoord2 );
            tcolor = colormap( unpackFloat( color2 ), u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
        }
        if ( u_mriSource == 3 )
        {
            color3 = texture( texture3, v_texCoord3 );
            tcolor = colormap( unpackFloat( color3 ), u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
        }
        if ( u_mriSource == 4 )
        {
            color4 = texture( texture4, v_texCoord4 );
            tcolor = colormap( unpackFloat( color4 ), u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
        }
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
    
    writePeel( vec4( light( vec4( color, 1.0 ) ).rgb, u_alpha ) );
}
