varying vec3 v_texcoord;

float unpackFloat(const vec4 value) {
    const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    //const vec4 bitSh = vec4(0.0, 0.0, 1.0, 1.0 / 256.0 );
    return (dot(value, bitSh));
}

void main()
{
	vec4 color;
	color = vec4( 0.0, 0.0, 0.0, 1.0 );

	if ( u_texActive0 )
	{
		vec4 color0 = texture3D( texture0, v_texcoord );
		if ( u_colormap0 == -1 )
		{
            color = color0;
        }
		else
		{ 
            color = colormap( unpackFloat( color0 ), u_colormap0, u_lowerThreshold0, u_upperThreshold0, u_selectedMin0, u_selectedMax0, 1.0, color );
        }
	}

	if ( u_texActive1 )
	{
		vec4 color1 = texture3D( texture1, v_texcoord );
		if ( u_colormap1 == -1 )
        {
            color = mix( color, color1, u_alpha1 );
        }
        else
        { 
            color = colormap( unpackFloat( color1 ), u_colormap1, u_lowerThreshold1, u_upperThreshold1, u_selectedMin1, u_selectedMax1, u_alpha1, color );
        }
	}
	
	if ( u_texActive2 )
	{
		vec4 color2 = texture3D( texture2, v_texcoord );
		if ( u_colormap2 == -1 )
        {
            color = mix( color, color2, u_alpha2 );
        }
        else
        { 
            color = colormap( unpackFloat( color2 ), u_colormap2, u_lowerThreshold2, u_upperThreshold2, u_selectedMin2, u_selectedMax2, u_alpha2, color );
        }
	}
	
	if ( u_texActive3 )
	{
		vec4 color3 = texture3D( texture3, v_texcoord );
		if ( u_colormap4 == -1 )
        {
            color = mix( color, color3, u_alpha3 );
        }
        else
        { 
            color = colormap( unpackFloat( color3 ), u_colormap3, u_lowerThreshold3, u_upperThreshold3, u_selectedMin3, u_selectedMax3, u_alpha3, color );
        }
	}
	
	if ( u_texActive4 )
	{
		vec4 color4 = texture3D( texture4, v_texcoord );
		if ( u_colormap4 == -1 )
        {
            color = mix( color, color4, u_alpha4 );
        }
        else
        { 
            colormap( unpackFloat( color4 ), u_colormap4, u_lowerThreshold4, u_upperThreshold4, u_selectedMin4, u_selectedMax4, u_alpha4, color );
        }
	}
	
	if ( !( color.r + color.g + color.b > 0.0 ) ) discard; 
	
	if ( u_picking )
	{
        gl_FragColor = u_pickColor;
    }
	else
	{
	   gl_FragColor = color;
	}   
	
}
