#version 330

#include uniforms_vs
#include lighting_vs
#include peel_vs

out vec3 vertex;

void main()
{
	v_normal = normalize( a_normal );
    v_value = a_value;
   
    prepareLight();
	
	frontColor = a_color;
	
	v_texcoord = vec3( ( a_position.x + u_dx / 2.0 ) / u_dims.x , ( a_position.y + u_dy / 2.0 ) / u_dims.y, ( a_position.z + u_dz / 2.0 ) / u_dims.z );
	
	v_position = mvp_matrix * vec4( a_position.x + u_adjustX, a_position.y + u_adjustY, a_position.z + u_adjustZ,  1.0 );
	
	vertex = a_position.xyz;
    gl_Position = v_position;
}
