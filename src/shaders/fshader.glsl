uniform sampler3D texture;

varying vec3 v_texcoord;


void main()
{
    gl_FragColor = texture3D(texture, v_texcoord);
}
