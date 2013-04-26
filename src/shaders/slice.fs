#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include peel_fs

void main()
{
    vec4 color = texColor( v_texcoord );
    if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
    
    if ( u_renderMode == 1 )
    {
    
        color = u_pickColor;
    }
	
	writePeel( color.rgb );   
}
