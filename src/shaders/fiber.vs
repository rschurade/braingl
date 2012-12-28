uniform mat4 mvp_matrix;
uniform mat4 mv_matrixInvert;

attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec4 a_color;

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
	v_normal = normalize( a_normal.xyz );
   
    v_viewDir = mv_matrixInvert * vec4( 0.0, 0.0, 1.0, 0.0);
    v_viewDir.w = 1.0;
    
    vec4 lightPos = vec4( 0.0, 0.0, 1.0, 0.0 );
    v_lightDir.xyz = normalize( ( mv_matrixInvert * lightPos ).xyz );
	
	gl_FrontColor = a_color;
	
    gl_Position = mvp_matrix * a_position;
}
