uniform mat4 mvp_matrix;

attribute vec4 a_position;
attribute vec4 a_normal;

varying vec3 v_normal;
varying vec3 v_lightDirection;

void main()
{
	v_lightDirection = vec3( 0, 0, -1.0 );
	
	v_normal = normalize( a_normal.xyz );
	
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;
}
