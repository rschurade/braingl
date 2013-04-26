#version 330

#include uniforms_fs

uniform vec4 u_color;
out vec4 fragColor;

void main()
{
	fragColor = vec4( u_color.rgb, 1.0 );
}
