#include colormap_fs
#include textures_fs

varying vec4 v_position;
varying vec3 v_normal;

varying float v_extra;
varying float v_sparam; // s parameter of texture [-1..1]
varying float v_tangent_dot_view;

uniform int u_colorMode;
uniform int u_colormap;
uniform float u_selectedMin;
uniform float u_selectedMax;
uniform float u_lowerThreshold;
uniform float u_upperThreshold;



void main()
{
    vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 ); 
    if ( u_colorMode == 3 )
    {
	    if ( u_texActive0 )
	    {
	        vec4 color0 = texture3D( texture0, v_texcoord );
	        if ( u_colormap0 == -1 )
	        {
	            color = color0;
	        }
	        else
	        { 
	            color = colormap( unpackFloat( color0 ), u_colormap0, u_lowerThreshold0, u_upperThreshold0, u_selectedMin0, u_selectedMax0, 1.0, color );
	        }
	    }
	
	    if ( u_texActive1 )
	    {
	        vec4 color1 = texture3D( texture1, v_texcoord );
	        if ( u_colormap1 == -1 )
	        {
	            color = mix( color, color1, u_alpha1 );
	        }
	        else
	        { 
	            color = colormap( unpackFloat( color1 ), u_colormap1, u_lowerThreshold1, u_upperThreshold1, u_selectedMin1, u_selectedMax1, u_alpha1, color );
	        }
	    }
	    
	    if ( u_texActive2 )
	    {
	        vec4 color2 = texture3D( texture2, v_texcoord );
	        if ( u_colormap2 == -1 )
	        {
	            color = mix( color, color2, u_alpha2 );
	        }
	        else
	        { 
	            color = colormap( unpackFloat( color2 ), u_colormap2, u_lowerThreshold2, u_upperThreshold2, u_selectedMin2, u_selectedMax2, u_alpha2, color );
	        }
	    }
	    
	    if ( u_texActive3 )
	    {
	        vec4 color3 = texture3D( texture3, v_texcoord );
	        if ( u_colormap4 == -1 )
	        {
	            color = mix( color, color3, u_alpha3 );
	        }
	        else
	        { 
	            color = colormap( unpackFloat( color3 ), u_colormap3, u_lowerThreshold3, u_upperThreshold3, u_selectedMin3, u_selectedMax3, u_alpha3, color );
	        }
	    }
	    
	    if ( u_texActive4 )
	    {
	        vec4 color4 = texture3D( texture4, v_texcoord );
	        if ( u_colormap4 == -1 )
	        {
	            color = mix( color, color4, u_alpha4 );
	        }
	        else
	        { 
	            color = colormap( unpackFloat( color4 ), u_colormap4, u_lowerThreshold4, u_upperThreshold4, u_selectedMin4, u_selectedMax4, u_alpha4, color );
	        }
	    }
	    if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
    }
    else if ( u_colorMode == 4 )
    {
        color = colormap( v_extra, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax, 1.0, color );
 
    }
    else
    {
        color = gl_Color;
    }
    float view_dot_normal = sqrt( 1. - v_sparam * v_sparam ) + .1;    
    color = clamp( view_dot_normal * ( color + 0.15 * pow( view_dot_normal, 10. ) * pow( v_tangent_dot_view, 10. ) ), 0., 1. );
    color.a = 1.0;
    gl_FragColor = color;
    
}
