#version 330

#include uniforms_vs

void main()
{
    // Calculate vertex position in screen space
    gl_Position = vec4( a_position, 1.0 );
    gl_Position.x = ( a_position.x * u_scaleX / u_width * 2.0 ) - 1.0;
    gl_Position.y = ( a_position.y * u_scaleY / u_height * 2.0 ) - 1.0;
}
