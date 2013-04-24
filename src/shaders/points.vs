uniform mat4 mvp_matrix;
uniform mat4 mv_matrixInvert;

uniform float threshold;
uniform float radius;
uniform float minlength;
uniform float u_scale;

attribute vec3 a_position;
attribute vec3 a_to;
attribute float a_value;
attribute vec3 dg;
attribute vec3 dc;

void main()
{
    vec3 d = abs(normalize( dc ));
    if ( threshold < a_value && length(a_position-a_to) > minlength) {
        gl_FrontColor =  vec4(d, 1.0 );
    } else {
        gl_FrontColor =  vec4(d, 0.0 );
    }

    gl_Position = mvp_matrix * vec4(a_position + radius*(dg), 1.0); 
    gl_Position += vec4(0,0,-0.00001*u_scale,0);
}
