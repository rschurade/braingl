#version 120

#include uniforms_vs
#include peel_vs

varying float v_discard;


vec4 getColor( vec3 vec )
{
    vec4 color;
    color = vec4( abs( vec ), 1. );
    return color;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// vertex shader -- main
//
/////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    float dir = a_dir;
    vec4 vec = vec4( a_vec, 0. ) * dir * u_scaling;
    
    gl_FrontColor = getColor( a_vec.xyz );
 
    v_discard = 0.;
    if ( length( a_vec.xyz ) < 0.0001 )
    {
        v_discard = 1.0;
    }
    
    v_position = mvp_matrix * ( vec4( a_position, 1.0 ) + vec );
    gl_Position = v_position;
}
