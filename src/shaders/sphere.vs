#include uniforms_vs
#include lighting_vs

void main()
{
    v_normal = a_position;

    v_viewDir = mv_matrixInvert * vec4( 0.0, 0.0, 1.0, 0.0);
    v_viewDir.w = 1.0;
    
    vec4 lightPos = vec4( 0.0, 0.0, 1.0, 0.0 );
    v_lightDir.xyz = normalize( ( mv_matrixInvert * lightPos ).xyz );

    vec3 pos = a_position;
    pos.x = ( pos.x * u_dx ) + u_x;
    pos.y = ( pos.y * u_dy ) + u_y;
    pos.z = ( pos.z * u_dz ) + u_z;
    
    gl_Position = mvp_matrix * vec4( pos, 1.0 );     
}
