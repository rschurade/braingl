#include textures_vs
#include uniforms_vs

void main()
{
    v_texcoord = a_texcoord;
    
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vec4( a_position, 1.0 );
}
