#version 330

#include uniforms_vs
#include textures_vs
#include peel_vs

void main()
{
    // Calculate vertex position in screen space
    v_position = mvp_matrix * vec4( a_position, 1.0 ); 
    gl_Position = v_position; 
    calcTexCoords();
}
