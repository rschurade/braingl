#version 120

#include uniforms_vs
#include peel_vs

void main()
{
    vec3 pos = a_position;
    pos.x = ( pos.x * u_dx ) + u_x;
    pos.y = ( pos.y * u_dy ) + u_y;
    pos.z = ( pos.z * u_dz ) + u_z;
    
    v_position = mvp_matrix * vec4( pos, 1.0 );
    
    gl_Position = v_position;      
}
