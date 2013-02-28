uniform mat4 p_matrix;
uniform mat4 mv_matrix;
uniform mat4 mv_matrixI;
uniform mat4 mv_matrixT;
uniform mat4 mv_matrixTI;
uniform mat4 mv_matrixIT;

uniform int u_colorMode;
uniform vec3 u_color;
uniform float u_thickness;

attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec4 a_color;
attribute float a_extra;
attribute float a_direction;

varying vec4 v_position;
varying vec3 v_normal;
varying float v_extra;

varying vec3 v_texcoord;
varying float v_sparam;

varying float v_tangent_dot_view;
varying float v_location;

void main()
{
    v_position = p_matrix * mv_matrix * a_position;
    
    v_normal = normalize( ( a_normal ).xyz );

    vec3 tangent = normalize( ( mv_matrixTI * vec4( v_normal, 1.0 ) ).xyz );
        
    vec3 view = vec3( 0.0, 0.0, -1.0 );
    
    vec3 offsetNN = cross( tangent, view );
    v_tangent_dot_view = length( offsetNN ); 
    
    vec3 offset = normalize( offsetNN );
    offset.x *= u_thickness;
    offset.y *= u_thickness;
    
    v_sparam = a_direction;
    v_position.xyz =  ( offset * v_sparam ) + v_position.xyz;

    gl_Position = v_position; //< store final position
	
	
	
	
	
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
    
    v_texcoord = vec3( a_position.x / 160.0, a_position.y / 200.0, a_position.z / 160.0 );
}
