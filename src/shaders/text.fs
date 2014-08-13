#version 330

uniform sampler2D fontTex;
in vec2 v_texCoord;

#include peel_fs

uniform vec3 u_textColor;

void main()
{
    vec4 color = texture( fontTex, v_texCoord );
    
    if ( color.a < 0.01 )
    {
        discard;
    }

    float mult = 1.0 - color.r;
    
    color.rgb = u_textColor * mult;
    
    writePeel( color );
}
