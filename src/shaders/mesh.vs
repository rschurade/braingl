#version 330

in vec3 a_position;
in vec3 a_normal;
in vec4 a_color;
in float a_value;

out vec3 v_vertex;
out vec3 v_normal;
out vec4 v_color;
out float v_value;

uniform mat4 mvp_matrix;
uniform mat4 userTransformMatrix;

uniform float u_adjustX;
uniform float u_adjustY;
uniform float u_adjustZ; 

#include textures_vs
#include lighting_vs
#include peel_vs

void main()
{
	v_normal = normalize( a_normal );
    v_value = a_value;
   
    prepareLight();
	
	v_color = a_color;
	
	calcTexCoords();
	
	v_position = mvp_matrix * userTransformMatrix * vec4( a_position.x + u_adjustX, a_position.y + u_adjustY, a_position.z + u_adjustZ,  1.0 );
	
	v_vertex = a_position.xyz;
    gl_Position = v_position;
}
