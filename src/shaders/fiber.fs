#include colormap_fs
#include textures_fs
#include uniforms_fs

varying float v_discard;

void main()
{
    if ( v_discard > 0.0 )
    {
        discard;
    }
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
