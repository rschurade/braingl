uniform mat4 mvp_matrix;
uniform mat4 mv_matrixInvert;

attribute vec4 a_position;
attribute vec3 a_offset;
attribute float a_radius;

uniform bool u_hideNegativeLobes;
uniform float u_scaling;

varying vec3 v_normal;

// point on projection plane of current pixel
// USAGE:
// x,y,z components:        the point
// w component:             unused
varying vec4 v_viewDir;

// light direction
// USAGE:
// x,y,z components:        the light direction vector
// w component:             unused
// (4 varying floats)
varying vec4 v_lightDir;


void main()
{
	v_normal = normalize( a_position.xyz );
	
	float r = a_radius;
	
	if ( u_hideNegativeLobes )
	{
		if ( r < 0.0 )
		{
		   r = 0.0;
		}
		gl_FrontColor = vec4(abs(v_normal), 1.0 );
    }
    else
    {
        if ( r < 0.0 )
        {
           gl_FrontColor = vec4( 0.5, 0.5, 0.5, 1.0 );
        }
        else
        {
            gl_FrontColor = vec4(abs(v_normal), 1.0 );
        }
    }
    
    v_viewDir = mv_matrixInvert * vec4( 0.0, 0.0, 1.0, 0.0);
    v_viewDir.w = 1.0;
    
    vec4 lightPos = vec4( 0.0, 0.0, 1.0, 0.0 );
    v_lightDir.xyz = normalize( ( mv_matrixInvert * lightPos ).xyz );
	
    // Calculate vertex position in screen space
    vec4 newPos = vec4( 1.0 );
    newPos.x = a_position.x * r * u_scaling + a_offset.x;
    newPos.y = a_position.y * r * u_scaling + a_offset.y;
    newPos.z = a_position.z * r * u_scaling + a_offset.z;
    gl_Position = mvp_matrix * newPos;
}
