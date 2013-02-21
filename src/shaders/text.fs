uniform sampler2D fontTex;

varying vec2 v_texCoord;

void main()
{
    vec4 color = texture2D( fontTex, v_texCoord );
    
    color.a = 1.0 - color.r;
    
	gl_FragColor = color;
}
