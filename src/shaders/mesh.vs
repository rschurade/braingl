#version 330

#include uniforms_vs
#include lighting_vs
#include peel_vs

void main()
{
	v_normal = normalize( a_normal );
    v_value = a_value;
   
    prepareLight();
	
	frontColor = a_color;
	
	v_texcoord = vec3( a_position.x / u_dims.x, a_position.y / u_dims.y, a_position.z / u_dims.z );
	
	v_position = mvp_matrix * vec4( a_position, 1.0 );
	
    gl_Position = v_position;
}
