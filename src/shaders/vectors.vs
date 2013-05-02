#version 330

#include uniforms_vs
#include peel_vs

uniform float threshold;
uniform float radius;
uniform float minlength;
uniform float u_scale;

in vec3 a_to;
in float a_flag;
in vec3 dg;
in vec3 dc;

out float v_discard;

void main()
{
    vec3 d = abs( normalize( dc ) );
    frontColor =  vec4(d, 1.0 );
    if ( threshold < a_value && length( a_position-a_to ) > minlength) {
        v_discard = 0.0;
    } else {
        v_discard = 1.0;
    }

    if ( a_flag > 0.0 ) {
	   v_position = mvp_matrix * vec4( a_position + radius*(dg), 1.0 );
	} else {
	   v_position = mvp_matrix * vec4( a_to, 1.0 );
	}
	v_position += vec4( 0, 0, -0.00001*u_scale, 0 );
    gl_Position = v_position;  

}
