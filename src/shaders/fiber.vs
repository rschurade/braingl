#version 330

#include uniforms_vs
#include peel_vs

out float v_discard;

void main()
{
	v_normal = normalize( a_normal );
	v_extra = a_extra;
	v_index = a_indexes;
	
	if ( u_colorMode == 0 )
	{
	   frontColor = u_globalColor;
	}
	else if ( u_colorMode == 1 )
	{
	   frontColor = vec4( abs( v_normal ), 1.0 );
	}
	else
    {
       frontColor =  vec4( u_color.xyz, 1.0 );
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
