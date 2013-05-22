#version 330

uniform sampler2D C0; // TEXTURE2 - opaque color map
uniform sampler2D C1; // TEXTURE6
uniform sampler2D C2; // TEXTURE7
uniform sampler2D C3; // TEXTURE8
uniform sampler2D C4; // TEXTURE9

uniform bool transparency_new;

in vec3 v_texcoord;

#include peel_fs

void main(void) {
    // need to combine colors from C0, C1, C2, C3, C4

    vec4 c0 = texture2D( C0, v_texcoord.xy );
    vec4 c1 = texture2D( C1, v_texcoord.xy );
    vec4 c2 = texture2D( C2, v_texcoord.xy );
    vec4 c3 = texture2D( C3, v_texcoord.xy );
    vec4 c4 = texture2D( C4, v_texcoord.xy );
    vec4 c5o = texture2D( C5, v_texcoord.xy );

float d = decode( texture2D( D1, v_texcoord.xy ) );
vec4 c5;
if ( d > 0 )
{
    float sumA = c5o.a / d;
    c5 = c5o;
    if (!transparency_new)
    {
       c5.a = c5o.a / ( sumA / 2 );
    }
}
else
{
   c5 = c5o;
}

    vec3 mcolor = c1.rgb*c1.a + 
                 (1.0-c1.a) * (c2.rgb*c2.a +
                 (1.0-c2.a) * (c3.rgb*c3.a +
                 (1.0-c3.a) * (c4.rgb*c4.a + 
                 (1.0-c4.a) * (c5.rgb*c5.a +
                 (1.0-c5.a) * c0.rgb))));
         
    fragColor = vec4 (mcolor, 1.0);
}
