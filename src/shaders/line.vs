#version 330

#include uniforms_vs

void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vec4( a_position, 1.0 );
    
    frontColor = a_color;
}
