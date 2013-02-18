uniform sampler3D texture0;
uniform sampler3D texture1;
uniform sampler3D texture2;
uniform sampler3D texture3;
uniform sampler3D texture4;

uniform bool u_texActive0;
uniform float u_alpha0;
uniform float u_lowerThreshold0;
uniform float u_upperThreshold0;
uniform float u_selectedMin0;
uniform float u_selectedMax0;
uniform int u_colormap0;

uniform bool u_texActive1;
uniform float u_alpha1;
uniform float u_lowerThreshold1;
uniform float u_upperThreshold1;
uniform float u_selectedMin1;
uniform float u_selectedMax1;
uniform int u_colormap1;

uniform bool u_texActive2;
uniform float u_alpha2;
uniform float u_lowerThreshold2;
uniform float u_upperThreshold2;
uniform float u_selectedMin2;
uniform float u_selectedMax2;
uniform int u_colormap2;

uniform bool u_texActive3;
uniform float u_alpha3;
uniform float u_lowerThreshold3;
uniform float u_upperThreshold3;
uniform float u_selectedMin3;
uniform float u_selectedMax3;
uniform int u_colormap3;

uniform bool u_texActive4;
uniform float u_alpha4;
uniform float u_lowerThreshold4;
uniform float u_upperThreshold4;
uniform float u_selectedMin4;
uniform float u_selectedMax4;
uniform int u_colormap4;

uniform bool u_picking;
uniform vec4 u_pickColor;

varying vec3 v_texcoord;

float unpackFloat(const vec4 value) {
    const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    //const vec4 bitSh = vec4(0.0, 0.0, 1.0, 1.0 / 256.0 );
    return (dot(value, bitSh));
}
