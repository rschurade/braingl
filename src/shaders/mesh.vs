#include lighting_vs
#include uniforms_vs

void main()
{
	v_normal = normalize( a_normal );
   
    v_viewDir = mv_matrixInvert * vec4( 0.0, 0.0, 1.0, 0.0);
    v_viewDir.w = 1.0;
    
    vec4 lightPos = vec4( 0.0, 0.0, 1.0, 0.0 );
    v_lightDir.xyz = normalize( ( mv_matrixInvert * lightPos ).xyz );
	
	gl_FrontColor = vec4( 1.0 );
	
	v_texcoord = vec3( a_position.x / u_dims.x, a_position.y / u_dims.y, a_position.z / u_dims.z );
	
    gl_Position = mvp_matrix * vec4( a_position, 1.0 );
}
