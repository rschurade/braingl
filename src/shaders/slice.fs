#version 120

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include peel_fs

void main()
{
    vec4 color = texColor( v_texcoord );
    if ( u_picking )
    {
        color = u_pickColor;
    }

	if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
	
	writePeel( color.rgb );   
}
