#version 330

#include uniforms_vs
#include textures_vs
#include lighting_vs
#include peel_vs

out vec3 vertex;

void main()
{
	v_normal = normalize( a_normal );
    v_value = a_value;
   
    prepareLight();
	
	frontColor = a_color;
	
	calcTexCoords();
	
	v_position = mvp_matrix * userTransformMatrix * vec4( a_position.x + u_adjustX, a_position.y + u_adjustY, a_position.z + u_adjustZ,  1.0 );
	
	vertex = a_position.xyz;
    gl_Position = v_position;
}
