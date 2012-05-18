uniform mat4 mvp_matrix;

attribute vec4 a_position;
attribute vec3 a_texcoord;

varying vec3 v_texcoord;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;
    
    v_texcoord = a_texcoord;
}
