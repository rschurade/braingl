uniform mat4 mvp_matrix;
uniform mat4 mv_matrixInvert;
uniform int u_colorMode;
uniform vec3 u_color;

attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec4 a_color;
attribute float a_extra;

varying vec3 v_normal;
varying float v_extra;

varying vec3 v_texcoord;

void main()
{
	v_normal = normalize( a_normal.xyz );
	v_extra = a_extra;
	
	if ( u_colorMode == 0 )
	{
	   gl_FrontColor = a_color;
	}
	else if ( u_colorMode == 1 )
	{
	   gl_FrontColor = vec4( abs( v_normal ), 1.0 );
	}
	else
    {
       gl_FrontColor =  vec4( u_color, 1.0 );
    }
    // TODO submit the actual dims in a uniform
    v_texcoord = vec3( a_position.x / 160.0, a_position.y / 200.0, a_position.z / 160.0 );
	
    gl_Position = mvp_matrix * a_position;
}
