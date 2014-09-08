out vec4 v_viewDir;
out vec4 v_lightDir;

uniform bool u_lighting;
uniform mat4 mv_matrixInvert;

void prepareLight()
{
    v_viewDir = mv_matrixInvert * vec4( 0.0, 0.0, 1.0, 0.0);
    v_viewDir.w = 1.0;
    
    vec4 lightPos = vec4( 0.0, 0.0, 1.0, 0.0 );
    v_lightDir.xyz = normalize( ( mv_matrixInvert * lightPos ).xyz );
}