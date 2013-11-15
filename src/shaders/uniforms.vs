in vec3 a_position;
in vec3 a_texcoord;
in vec3 a_normal;
in vec4 a_color;

in float a_extra;
in float a_indexes;
in float a_value;
in float a_dir;
in vec3 a_vec;
in vec3 a_offset;
in float a_radius;
in vec3 a_diag;
in vec3 a_offdiag;

out vec3 v_texcoord;
out vec3 v_normal;
out float v_extra;
out vec4 frontColor;
out float v_value;
out float v_index;

uniform mat4 mv_matrixTI;
uniform mat4 mvp_matrix;
uniform mat4 mv_matrixInvert;

uniform int u_colorMode;
uniform vec4 u_color;
uniform vec4 u_globalColor;

uniform vec3 u_dims;

uniform float u_x;
uniform float u_y;
uniform float u_z;
uniform float u_dx;
uniform float u_dy;
uniform float u_dz;

uniform float u_cutx;
uniform float u_cuty;
uniform float u_cutz;
uniform float u_cutdx;
uniform float u_cutdy;
uniform float u_cutdz;

uniform float u_width;
uniform float u_height;
uniform float u_scaleX;
uniform float u_scaleY;

uniform float u_scaling;

uniform bool u_hideNegativeLobes;

uniform float u_sizeX;
uniform float u_sizeY;
uniform float u_pos;
uniform float u_char;

uniform float u_adjustX;
uniform float u_adjustY;
uniform float u_adjustZ; 
