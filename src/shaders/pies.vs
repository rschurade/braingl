#version 330

#include uniforms_vs
#include peel_vs

uniform mat4 p_matrix;

uniform float threshold;
uniform float minlength;
uniform float radius;
uniform float normalization;
uniform float u_scale;

in vec3 a_diff;
in float a_nth;
in float a_cover;

out float v_discard;
flat out vec4 pieColor;

void main()
{
    float PI = 3.14159265358979323846264;
    float t = a_nth/a_cover*2*PI;
    vec3 d = abs( normalize( a_diff ) );
    pieColor =  vec4(d, 1.0 );
     
    v_position = mvp_matrix * vec4(a_position, 1.0)+vec4(0,0,-0.00001*u_scale,0);
    if (a_nth > -0.5) {
      float r = (normalization*a_cover + (1-normalization)*100)*radius;
      v_position += vec4(0.001*u_scale*p_matrix[0][0]*r*sin(t),0.001*u_scale*p_matrix[1][1]*r*cos(t),0,0);
    }
    
    gl_Position = v_position;  

}
