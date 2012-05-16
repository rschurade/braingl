uniform sampler3D texture0;
uniform sampler3D texture1;
uniform sampler3D texture2;

uniform float u_lowerThreshold;
uniform float u_upperThreshold;

varying vec3 v_texcoord;


void main()
{
	if( texture3D(texture0, v_texcoord).r < u_lowerThreshold )
		discard;
	if( texture3D(texture0, v_texcoord).r > u_upperThreshold )
		discard;
		
    gl_FragColor = texture3D(texture0, v_texcoord);
}
