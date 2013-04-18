#include uniforms_vs
#include peel_vs

varying float v_discard;

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
    v_discard = 0.0;
    if ( a_position.x <= ( u_x - u_dx ) || a_position.x >= ( u_x + u_dx ) || 
         a_position.y <= ( u_y - u_dy ) || a_position.y >= ( u_y + u_dy ) ||
         a_position.z <= ( u_z - u_dz ) || a_position.z >= ( u_z + u_dz ) )
    {
        v_discard = 1.0;
    } 
    
    // TODO submit the actual dims in a uniform
    v_texcoord = vec3( a_position.x / 160.0, a_position.y / 200.0, a_position.z / 160.0 );
	
	v_position = mvp_matrix * vec4( a_position, 1.0 );
    gl_Position = v_position;
}
