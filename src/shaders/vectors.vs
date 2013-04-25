#include uniforms_vs
#include peel_vs

uniform float threshold;
uniform float radius;
uniform float minlength;
uniform float u_scale;

attribute vec3 a_to;
attribute float a_flag;
attribute vec3 dg;
attribute vec3 dc;

varying float v_discard;

void main()
{
    vec3 d = abs( normalize( dc ) );
    gl_FrontColor =  vec4(d, 1.0 );
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
