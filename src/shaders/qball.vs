uniform mat4 mvp_matrix;

attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec3 a_offset;
attribute float a_radius;

varying vec3 v_normal;
varying vec3 v_lightDirection;

void main()
{
	v_lightDirection = vec3( 0, 0, -1.0 );
	
	v_normal = normalize( a_normal.xyz );
	
    // Calculate vertex position in screen space
    vec4 newPos = vec4( 1.0 );
    newPos.x = a_position.x * a_radius + a_offset.x;
    newPos.y = a_position.y * a_radius + a_offset.y;
    newPos.z = a_position.z * a_radius + a_offset.z;
    gl_Position = mvp_matrix * newPos;
}
