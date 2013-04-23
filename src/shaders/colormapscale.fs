uniform vec4 u_color;

#include peel_fs

void main()
{
	vec3 color = vec3( 0.0 );
	writePeel( color );
}
