#include uniforms_vs
#include peel_vs

void main()
{
    // Calculate vertex position in screen space
    
    v_position = vec4( a_position, 1.0 );
    v_position.x = ( a_position.x * u_scaleX / u_width * 2.0 ) - 1.0;
    v_position.y = ( a_position.y * u_scaleY / u_height * 2.0 ) - 1.0;
    
    v_texcoord = a_texcoord;
    gl_Position = v_position;
}
