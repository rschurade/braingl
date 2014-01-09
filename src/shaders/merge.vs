#version 330

in vec3 a_position;
in vec3 a_texcoord;

out vec3 v_texcoord;

void main(void) {
    v_texcoord = a_texcoord;

    gl_Position = vec4( a_position, 1.0 );
}