#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include peel_fs

in float v_sparam; // s parameter of texture [-1..1]
in float v_tangent_dot_view;

in float v_discard;
uniform float u_fibGrowth;
uniform int u_mriSource;

void main()
{
    if ( v_index > u_fibGrowth )
    {
        discard;
    }

    vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 ); 
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
        
        color = tcolor;
    }
    else if ( u_colorMode == 4 )
    {
        color = colormap( v_extra, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
 
    }
    else
    {
        color = frontColor;
    }
    
//    int i_index = int(  v_index );    
//    int i_fibGrowth = int( u_fibGrowth );
//    if ( ( i_index % 50 ) > ( i_fibGrowth % 50 ) -1  &&
//         ( i_index % 50 ) < ( i_fibGrowth % 50 ) +1 )
//    {
//        color = vec4( 1.0 );
//    }
        
    float view_dot_normal = sqrt( 1. - v_sparam * v_sparam ) + .1;    
    color = clamp( view_dot_normal * ( color + 0.15 * pow( view_dot_normal, 10. ) * pow( v_tangent_dot_view, 10. ) ), 0., 1. );
    color.a = 1.0;
    
    writePeel( vec4( color.rgb, u_alpha ) );
}
