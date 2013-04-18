#include textures_vs
#include uniforms_vs
#include peel_vs

void main()
{
    v_texcoord = a_texcoord;
    
    // Calculate vertex position in screen space
    v_position = mvp_matrix * vec4( a_position, 1.0 ); 
    gl_Position = v_position; 
}
