#version 330

#include uniforms_vs
#include lighting_vs
#include peel_vs

out float v_discard;

void main()
{
	v_normal = normalize( a_normal );
    v_value = a_value;
   
    prepareLight();
	
	frontColor = a_color;
	
	v_texcoord = vec3( a_position.x / u_dims.x, a_position.y / u_dims.y, a_position.z / u_dims.z );
	
	v_position = mvp_matrix * vec4( a_position, 1.0 );
	
	v_discard = 0.0;
	if ( u_cutLowerX && a_position.x < u_x )
	{
		if ( u_cutLowerY && a_position.y < u_y )
		{
			if ( u_cutLowerZ && a_position.z < u_z )
			{
				v_discard = 1.0;
			}
			if ( u_cutHigherZ && a_position.z > u_z )
			{
				v_discard = 1.0;
			}
		}
		if ( u_cutHigherY && a_position.y > u_y )
		{
			if ( u_cutLowerZ && a_position.z < u_z )
			{
				v_discard = 1.0;
			}
			if ( u_cutHigherZ && a_position.z > u_z )
			{
				v_discard = 1.0;
			}
		}
	}	
	if ( u_cutHigherX && a_position.x > u_x )
	{
		if ( u_cutLowerY && a_position.y < u_y )
		{
			if ( u_cutLowerZ && a_position.z < u_z )
			{
				v_discard = 1.0;
			}
			if ( u_cutHigherZ && a_position.z > u_z )
			{
				v_discard = 1.0;
			}
		}
		if ( u_cutHigherY && a_position.y > u_y )
		{
			if ( u_cutLowerZ && a_position.z < u_z )
			{
				v_discard = 1.0;
			}
			if ( u_cutHigherZ && a_position.z > u_z )
			{
				v_discard = 1.0;
			}
		}
	}	
    gl_Position = v_position;
}
