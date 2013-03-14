#include colormap_fs
#include textures_fs

varying vec3 v_normal;
varying float v_extra;

uniform int u_colorMode;
uniform int u_colormap;
uniform float u_selectedMin;
uniform float u_selectedMax;
uniform float u_lowerThreshold;
uniform float u_upperThreshold;

void main()
{
    if ( u_colorMode == 3 )
    {
        vec4 color = texColor( v_texcoord );
	    
	    if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
	    
	    gl_FragColor = color;
    }
    else if ( u_colorMode == 4 )
    {
        vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 );
        gl_FragColor = colormap( v_extra, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax, 1.0, color );
    }
    else
    {
        gl_FragColor = gl_Color;
    }
}
