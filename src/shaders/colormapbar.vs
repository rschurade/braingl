attribute vec4 a_position;
attribute vec3 a_texcoord;

uniform float u_width;
uniform float u_height;
uniform float u_scaleX;
uniform float u_scaleY;

varying vec3 v_texcoord;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = a_position;
    gl_Position.x = ( a_position.x * u_scaleX / u_width * 2.0 ) - 1.0;
    gl_Position.y = ( a_position.y * u_scaleY / u_height * 2.0 ) - 1.0;
    
    v_texcoord = a_texcoord;
}
