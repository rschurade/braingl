// point on projection plane of current pixel
// USAGE:
// x,y,z components:        the point
// w component:             unused
out vec4 v_viewDir;

// light direction
// USAGE:
// x,y,z components:        the light direction vector
// w component:             unused
// (4 varying floats)
out vec4 v_lightDir;


uniform bool u_lighting;

void prepareLight()
{
    v_viewDir = mv_matrixInvert * vec4( 0.0, 0.0, 1.0, 0.0);
    v_viewDir.w = 1.0;
    
    vec4 lightPos = vec4( 0.0, 0.0, 1.0, 0.0 );
    v_lightDir.xyz = normalize( ( mv_matrixInvert * lightPos ).xyz );
}