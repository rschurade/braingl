#include uniforms_vs
#include lighting_vs

void main()
{
	v_normal = normalize( a_normal );
   
    prepareLight();
	
	gl_FrontColor = vec4( 1.0 );
	
	v_texcoord = vec3( a_position.x / u_dims.x, a_position.y / u_dims.y, a_position.z / u_dims.z );
	
    gl_Position = mvp_matrix * vec4( a_position, 1.0 );
}
