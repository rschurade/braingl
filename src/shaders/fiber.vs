#version 330

#include uniforms_vs
#include lighting_vs
#include peel_vs

out float v_discard;

void main()
{
	v_normal = normalize( a_normal );
	v_extra = a_extra;
	v_index = a_indexes;
	
	prepareLight();
	
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
    if ( a_position.x <= ( u_cutx - u_cutdx ) || a_position.x >= ( u_cutx + u_cutdx ) || 
         a_position.y <= ( u_cuty - u_cutdy ) || a_position.y >= ( u_cuty + u_cutdy ) ||
         a_position.z <= ( u_cutz - u_cutdz ) || a_position.z >= ( u_cutz + u_cutdz ) )
    {
        v_discard = 1.0;
    } 
    
    // TODO submit the actual dims in a uniform
    v_texcoord = vec3( ( a_position.x + u_dx / 2.0 ) / u_dims.x , ( a_position.y + u_dy / 2.0 ) / u_dims.y, ( a_position.z + u_dz / 2.0 ) / u_dims.z );
	
	v_position = mvp_matrix * vec4( a_position, 1.0 );
    gl_Position = v_position;
}
