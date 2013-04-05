#include colormap_fs
#include textures_fs
#include uniforms_fs
#include lighting_fs

varying float v_value;

void main()
{
    if ( u_colorMode == 1 )
    {
        vec4 color = texColor( v_texcoord );
    
        if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
        
        gl_FragColor = color;
    }
    else if ( u_colorMode == 3 )
    {
        vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 );
        color = colormap( v_value, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax, 1.0, color );
        light( color );
    }
    else
    {
        light( u_color );
    }    
}
