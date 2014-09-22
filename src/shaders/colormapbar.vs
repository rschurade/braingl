#version 330

#include uniforms_vs
#include peel_vs

void main()
{
    // Calculate vertex position in screen space
    
    v_position = vec4( a_position, 1.0 );
    v_position.x = ( a_position.x / u_width  * 2.0 * ( u_width  / 1000 ) ) - 1.0;
    v_position.y = ( a_position.y / u_height * 2.0 * ( u_height / 1000 ) ) - 1.0;
    
    v_texcoord = a_texcoord;
    gl_Position = v_position;
}
