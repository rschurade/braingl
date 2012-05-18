uniform sampler3D texture0;
uniform sampler3D texture1;
uniform sampler3D texture2;
uniform sampler3D texture3;
uniform sampler3D texture4;

uniform bool u_texActive0;
uniform float u_alpha0;
uniform float u_lowerThreshold0;
uniform float u_upperThreshold0;

uniform bool u_texActive1;
uniform float u_alpha1;
uniform float u_lowerThreshold1;
uniform float u_upperThreshold1;

uniform bool u_texActive2;
uniform float u_alpha2;
uniform float u_lowerThreshold2;
uniform float u_upperThreshold2;

uniform bool u_texActive3;
uniform float u_alpha3;
uniform float u_lowerThreshold3;
uniform float u_upperThreshold3;

uniform bool u_texActive4;
uniform float u_alpha4;
uniform float u_lowerThreshold4;
uniform float u_upperThreshold4;

varying vec3 v_texcoord;


void main()
{
//	vec4 color;
//	color = vec4( 0.0, 0.0, 0.0, 1.0 );
	if ( u_texActive0 )
	{
		if( texture3D( texture0, v_texcoord ).r < u_lowerThreshold0 )
			discard;
		if( texture3D( texture0, v_texcoord ).r > u_upperThreshold0 )
			discard;
		gl_FragColor = texture3D( texture0, v_texcoord );
	}
	//gl_FragColor = color;
}
