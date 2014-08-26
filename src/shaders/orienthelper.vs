#version 330

#include uniforms_vs
#include lighting_vs

#include peel_vs

void main()
{
    v_normal = a_position;
    
    if ( u_lighting )
    {
        prepareLight();
    }

    vec3 pos = a_position;
    pos.x = ( pos.x * u_dx );
    pos.y = ( pos.y * u_dx );
    pos.z = ( pos.z * u_dx );
    
    v_position = mvp_matrix * vec4( pos, 1.0 );
    v_position.x += u_x;
    v_position.y += u_y;
    v_position.z += u_z;
    
    gl_Position = v_position;     
}
