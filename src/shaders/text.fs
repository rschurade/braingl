uniform vec4 u_color;

uniform sampler2D fontTex;
varying vec2 v_texCoord;

#include peel_fs

void main()
{
    vec4 color = texture2D( fontTex, v_texCoord );
    
    color.a = 1.0 - color.r;
    
	writePeel( color.rgb );
}
