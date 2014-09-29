#version 330

#include uniforms_vs
#include peel_vs

void main()
{
	v_normal = a_normal;
	
	float r = a_radius;
	
	if ( u_hideNegativeLobes )
	{
		if ( r < 0.0 )
		{
		   r = 0.0;
		}
		frontColor = vec4(abs(v_normal), 1.0 );
    }
    else
    {
        if ( r < 0.0 )
        {
           frontColor = vec4( 0.5, 0.5, 0.5, 1.0 );
        }
        else
        {
            frontColor = vec4(abs(v_normal), 1.0 );
        }
    }
	
    // Calculate vertex position in screen space
    vec4 newPos = vec4( 1.0 );
    newPos.x = a_position.x * r * u_scaling + a_offset.x;
    newPos.y = a_position.y * r * u_scaling + a_offset.y;
    newPos.z = a_position.z * r * u_scaling + a_offset.z;
    
    v_position = mvp_matrix * newPos;
    gl_Position = v_position;
}
