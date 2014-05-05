#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include peel_fs

void main()
{
    vec4 color = texColor();
    
    if ( !( length( color.rgb ) > 0.0 ) ) 
    {
        discard;
    }
	
	writePeel( color );   
}
