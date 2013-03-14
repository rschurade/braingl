attribute vec3 a_position;
attribute vec3 a_texcoord;

uniform mat4 mvp_matrix;

varying vec3 v_texcoord;

void main()
{
    v_texcoord = a_texcoord;
    
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vec4( a_position, 1.0 );
}
