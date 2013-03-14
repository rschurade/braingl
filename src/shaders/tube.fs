#include colormap_fs
#include textures_fs
#include uniforms_fs

varying float v_sparam; // s parameter of texture [-1..1]
varying float v_tangent_dot_view;

void main()
{
    vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 ); 
    if ( u_colorMode == 3 )
    {
        color = texColor( v_texcoord );
	    
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
