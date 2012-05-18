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
	vec4 color;
	color = vec4( 0.0, 0.0, 0.0, 1.0 );

	if ( u_texActive0 )
	{
		vec4 color0 = texture3D( texture0, v_texcoord );
		if( color0.r > u_lowerThreshold0 && color0.r <= u_upperThreshold0 )
		{
			color = color0;
		}
	}

	if ( u_texActive1 )
	{
		vec4 color1 = texture3D( texture1, v_texcoord );
		if( color1.r > u_lowerThreshold1 && color1.r <= u_upperThreshold1 )
		{
			color.rgb =  mix( color.rgb, color1.rgb, u_alpha1 );
		}
	}
	
	if ( u_texActive2 )
	{
		vec4 color2 = texture3D( texture2, v_texcoord );
		if( color2.r > u_lowerThreshold2 && color2.r <= u_upperThreshold2 )
		{
			color.rgb =  mix( color.rgb, color2.rgb, u_alpha2 );
		}
	}
	
	if ( u_texActive3 )
	{
		vec4 color3 = texture3D( texture3, v_texcoord );
		if( color3.r > u_lowerThreshold3 && color3.r <= u_upperThreshold3 )
		{
			color.rgb =  mix( color.rgb, color3.rgb, u_alpha3 );
		}
	}
	
	if ( u_texActive4 )
	{
		vec4 color4 = texture3D( texture4, v_texcoord );
		if( color4.r > u_lowerThreshold4 && color4.r <= u_upperThreshold4 )
		{
			color.rgb =  mix( color.rgb, color4.rgb, u_alpha4 );
		}
	}

	if ( color.r < 0.00001 ) discard; 
	gl_FragColor = color;
}
