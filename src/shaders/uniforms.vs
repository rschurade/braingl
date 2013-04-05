attribute vec3 a_position;
attribute vec3 a_texcoord;
attribute vec3 a_normal;
attribute vec4 a_color;

attribute float a_extra;
attribute float a_value;
attribute float a_dir;
attribute vec3 a_vec;
attribute vec3 a_offset;
attribute float a_radius;
attribute vec3 a_diag;
attribute vec3 a_offdiag;

uniform mat4 mv_matrixTI;
uniform mat4 mvp_matrix;
uniform mat4 mv_matrixInvert;

uniform int u_colorMode;
uniform vec4 u_color;

uniform vec3 u_dims;

uniform float u_x;
uniform float u_y;
uniform float u_z;
uniform float u_dx;
uniform float u_dy;
uniform float u_dz;

uniform float u_width;
uniform float u_height;
uniform float u_scaleX;
uniform float u_scaleY;

uniform float u_scaling;

varying vec3 v_texcoord;
varying vec3 v_normal;
varying float v_extra;

uniform bool u_hideNegativeLobes;

uniform float u_sizeX;
uniform float u_sizeY;
uniform float u_pos;
uniform float u_char;
