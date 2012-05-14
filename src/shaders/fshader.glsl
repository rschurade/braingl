uniform sampler3D texture0;
uniform sampler3D texture1;
uniform sampler3D texture2;

varying vec3 v_texcoord;


void main()
{
    gl_FragColor = texture3D(texture0, v_texcoord);
}
