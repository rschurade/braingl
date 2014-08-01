
uniform int u_colorMode;
uniform int u_colormap;
uniform float u_min;
uniform float u_max;
uniform float u_selectedMin;
uniform float u_selectedMax;
uniform float u_lowerThreshold;
uniform float u_upperThreshold;

in vec3 v_texcoord;
in vec3 v_normal;
in float v_extra;
in vec4 frontColor;
in float v_index;

uniform float u_x;
uniform float u_y;
uniform float u_z;

uniform bool u_cutLowerX;
uniform bool u_cutLowerY;
uniform bool u_cutLowerZ;
uniform bool u_cutHigherX;
uniform bool u_cutHigherY;
uniform bool u_cutHigherZ;

uniform int u_meshTransparency;