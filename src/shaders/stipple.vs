#version 330

#include uniforms_vs
#include peel_vs

out vec3 v_dir;
out vec3 v_textPos;
out vec3 v_texCoord;

uniform float u_glyphSize;
uniform int u_orient;

vec4 getColor( vec3 vec )
{
    vec4 color;
    color = vec4( abs( vec ), 1. );
    return color;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// vertex shader -- main
//
/////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    v_dir = a_vec;
    frontColor = a_color;
    
    float vx = a_dir2.x;
    float vy = a_dir2.y;

    vec4 vec = vec4( 0. );
    if ( u_orient == 0 )
    {
        vec.x = vx;
        vec.y = vy;
    }
    if ( u_orient == 1 )
    {
        vec.x = vx;
        vec.z = vy;
    }
    if ( u_orient == 2 )
    {
        vec.y = vx;
        vec.z = vy;
    }
    vec *= u_glyphSize;
    
    vec3 texCoord = vec3( 0.0, 0.0, 0.0 );
    if ( vx > 0 )
    {
        texCoord.x = 1.0;
    }
    
    if ( vy > 0 )
    {
        texCoord.y = 1.0;
    }
    v_texCoord = texCoord;
    
    v_position = mvp_matrix * ( vec4( a_position, 1.0 ) + vec );
    v_textPos = a_position.xyz;
    gl_Position = v_position;
}
