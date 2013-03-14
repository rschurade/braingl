#include uniforms_vs

void main()
{
	v_normal = normalize( a_normal );
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
       gl_FrontColor =  vec4( u_color.xyz, 1.0 );
    }
    // TODO submit the actual dims in a uniform
    v_texcoord = vec3( a_position.x / 160.0, a_position.y / 200.0, a_position.z / 160.0 );
	
    gl_Position = mvp_matrix * vec4( a_position, 1.0 );
}
