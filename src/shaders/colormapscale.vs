#version 330

#include uniforms_vs

void main()
{
    // Calculate vertex position in screen space
    gl_Position = vec4( a_position, 1.0 );
    gl_Position.x = ( a_position.x / u_width *  2.0 * ( u_width / 1000 ) )  - 1.0;
    gl_Position.y = ( a_position.y / u_height * 2.0 * ( u_height / 1000 ) ) - 1.0;
}
