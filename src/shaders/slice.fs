uniform sampler3D texture0;
uniform sampler3D texture1;
uniform sampler3D texture2;
uniform sampler3D texture3;
uniform sampler3D texture4;

uniform bool u_texActive0;
uniform float u_alpha0;
uniform float u_lowerThreshold0;
uniform float u_upperThreshold0;
uniform int u_colormap0;

uniform bool u_texActive1;
uniform float u_alpha1;
uniform float u_lowerThreshold1;
uniform float u_upperThreshold1;
uniform int u_colormap1;

uniform bool u_texActive2;
uniform float u_alpha2;
uniform float u_lowerThreshold2;
uniform float u_upperThreshold2;
uniform int u_colormap2;

uniform bool u_texActive3;
uniform float u_alpha3;
uniform float u_lowerThreshold3;
uniform float u_upperThreshold3;
uniform int u_colormap3;

uniform bool u_texActive4;
uniform float u_alpha4;
uniform float u_lowerThreshold4;
uniform float u_upperThreshold4;
uniform int u_colormap4;

varying vec3 v_texcoord;

float unpackFloat(const vec4 value) {
    const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    //const vec4 bitSh = vec4(0.0, 0.0, 1.0, 1.0 / 256.0 );
    return (dot(value, bitSh));
}

vec3 colormap( vec3 v, int colormapSelection )
{
	if ( colormapSelection == 1 )
	{
		float value = v.r * 5.0;
        vec3 color;
        if( value < 0.0 )
        	color = vec3( 0.0, 0.0, 0.0 );
    	else if( value < 1.0 )
			color = vec3( 0.0, value, 1.0 );
        else if( value < 2.0 )
			color = vec3( 0.0, 1.0, 2.0-value );
    	else if( value < 3.0 )
			color =  vec3( value-2.0, 1.0, 0.0 );
    	else if( value < 4.0 )
			color = vec3( 1.0, 4.0-value, 0.0 );
    	else if( value <= 5.0 )
			color = vec3( 1.0, 0.0, value-4.0 );
    	else
			color =  vec3( 1.0, 0.0, 1.0 );
        return color;	
	}
	else
	{
		return vec3( v.r, v.r, v.r );
	}
}


void main()
{
	vec4 color;
	color = vec4( 0.0, 0.0, 0.0, 1.0 );

	if ( u_texActive0 )
	{
		vec4 color0 = texture3D( texture0, v_texcoord );
		float value0 = unpackFloat( color0 );
		if( value0 > u_lowerThreshold0 && value0 <= u_upperThreshold0 )
		{
		    value0 = ( value0 - u_lowerThreshold0 ) / ( u_upperThreshold0 - u_lowerThreshold0 );
		    color0.r = value0;
			color = vec4( colormap( color0.rgb, u_colormap0 ), 1.0 );
		}
	}

	if ( u_texActive1 )
	{
		vec4 color1 = texture3D( texture1, v_texcoord );
		float value1 = unpackFloat( color1 );
		if( value1 > u_lowerThreshold1 && value1 <= u_upperThreshold1 )
		{
		    value1 = ( value1 - u_lowerThreshold1 ) / ( u_upperThreshold1 - u_lowerThreshold1 );
            color1.r = value1;
			color.rgb =  mix( color.rgb, colormap( color1.rgb, u_colormap1 ), u_alpha1 );
		}
	}
	
	if ( u_texActive2 )
	{
		vec4 color2 = texture3D( texture2, v_texcoord );
		float value2 = unpackFloat( color2 );
		if( value2 > u_lowerThreshold2 && value2 <= u_upperThreshold2 )
		{
		    value2 = ( value2 - u_lowerThreshold2 ) / ( u_upperThreshold2 - u_lowerThreshold2 );
            color2.r = value2;
			color.rgb =  mix( color.rgb, colormap( color2.rgb, u_colormap2 ), u_alpha2 );
		}
	}
	
	if ( u_texActive3 )
	{
		vec4 color3 = texture3D( texture3, v_texcoord );
		float value3 = unpackFloat( color3 );
		if( value3 > u_lowerThreshold3 && value3 <= u_upperThreshold3 )
		{
		    value3 = ( value3 - u_lowerThreshold3 ) / ( u_upperThreshold3 - u_lowerThreshold3 );
            color3.r = value3;
			color.rgb =  mix( color.rgb, colormap( color3.rgb, u_colormap3 ), u_alpha3 );
		}
	}
	
	if ( u_texActive4 )
	{
		vec4 color4 = texture3D( texture4, v_texcoord );
		float value4 = unpackFloat( color4 );
		if( value4 > u_lowerThreshold4 && value4 <= u_upperThreshold4 )
		{
		    value4 = ( value4 - u_lowerThreshold4 ) / ( u_upperThreshold4 - u_lowerThreshold4 );
            color4.r = value4;
			color.rgb =  mix( color.rgb, colormap( color4.rgb, u_colormap4 ), u_alpha4 );
		}
	}

	if ( color.r + color.g + color.b < 0.00001 ) discard; 
	gl_FragColor = color;
}
