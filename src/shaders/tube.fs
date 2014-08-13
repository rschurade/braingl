#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include peel_fs

in float v_sparam; // s parameter of texture [-1..1]
in float v_tangent_dot_view;

in float v_discard;
uniform float u_fibGrowth;


void main()
{
    if ( v_index > u_fibGrowth )
    {
        discard;
    }

    vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 ); 
    if ( u_colorMode == 3 )
    {
        color = texColor();
	    
	    if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
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
