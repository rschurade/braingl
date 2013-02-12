uniform sampler3D texture0;
uniform sampler3D texture1;
uniform sampler3D texture2;
uniform sampler3D texture3;
uniform sampler3D texture4;

uniform bool u_texActive0;
uniform float u_alpha0;
uniform float u_lowerThreshold0;
uniform float u_upperThreshold0;
uniform float u_selectedMin0;
uniform float u_selectedMax0;
uniform int u_colormap0;

uniform bool u_texActive1;
uniform float u_alpha1;
uniform float u_lowerThreshold1;
uniform float u_upperThreshold1;
uniform float u_selectedMin1;
uniform float u_selectedMax1;
uniform int u_colormap1;

uniform bool u_texActive2;
uniform float u_alpha2;
uniform float u_lowerThreshold2;
uniform float u_upperThreshold2;
uniform float u_selectedMin2;
uniform float u_selectedMax2;
uniform int u_colormap2;

uniform bool u_texActive3;
uniform float u_alpha3;
uniform float u_lowerThreshold3;
uniform float u_upperThreshold3;
uniform float u_selectedMin3;
uniform float u_selectedMax3;
uniform int u_colormap3;

uniform bool u_texActive4;
uniform float u_alpha4;
uniform float u_lowerThreshold4;
uniform float u_upperThreshold4;
uniform float u_selectedMin4;
uniform float u_selectedMax4;
uniform int u_colormap4;

varying vec3 v_texcoord;

float unpackFloat(const vec4 value) {
    const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    //const vec4 bitSh = vec4(0.0, 0.0, 1.0, 1.0 / 256.0 );
    return (dot(value, bitSh));
}

vec4 colormap( vec4 v, int cmap, float lowerThreshold, float upperThreshold, float selectedMin, float selectedMax, float alpha, vec4 fragColor )
{
    vec3 color = vec3(0.0);
    if ( cmap == 4 ) // no float packing;
    {
         color = vec3( v.r, v.g, v.b );
         return vec4( mix( fragColor.rgb, color, alpha ), 1.0 ); 
    }

    float value = unpackFloat( v );

    if ( value < lowerThreshold )
    {
        return fragColor;
    }
    
    if ( value > upperThreshold )
    {
        return fragColor;
    }
    
    value = ( value - selectedMin ) / ( selectedMax - selectedMin );
    
    if ( cmap == 0 )
    {
        value = max( value, 0.001 );
        color = vec3( value, value, value );
    }
    if ( cmap == 1 )
    {
        value *= 5.0;
        if( value < 0.0 )
            color = vec3( 0.0, 0.0, 1.0 );
        else if( value < 1.0 )
            color = vec3( 0.0, value, 1.0 );
        else if( value < 2.0 )
            color = vec3( 0.0, 1.0, 2.0-value );
        else if( value < 3.0 )
            color = vec3( value-2.0, 1.0, 0.0 );
        else if( value < 4.0 )
            color = vec3( 1.0, 4.0-value, 0.0 );
        else if( value <= 5.0 )
            color = vec3( 1.0, 0.0, value-4.0 );
        else
            color =  vec3( 1.0, 0.0, 1.0 );
    }  
    if ( cmap == 2 )
    {
        value *= 6.0;
        if( value < 0.0 )
            color = vec3( 0.0, 0.0, 1.0 );
        else if( value < 2.0 )
            color = vec3( 0.0, .5 * value, 1.0 );
        else if( value < 3.0 )
            color = vec3( 0.0, 1.0, 3.0-value );
        else if( value < 4.0 )
            color =  vec3( value-3.0, 1.0, 0.0 );
        else if( value <= 6.0 )
            color = vec3( 1.0, .5 * (6.0-value), 0.0 );
        else
            color =  vec3( 1.0, 0.0, 0.0 ); 
    }
    if ( cmap == 3 )
    {
        value *= 2.0;
        if( value < 0.0 )
            color = vec3( 0.0, 0.0, 1.0 );
        else if( value < 1.0 )
            color = vec3( value, value, 1.0 );
        else if( value <= 2.0 )
            color = vec3( 1.0, 2.0-value, 2.0-value );
        else
            color =  vec3( 1.0, 0.0, 0.0 );  
    }
    return vec4( mix( fragColor.rgb, color, alpha ), 1.0 ); 
} 

void main()
{
	vec4 color;
	color = vec4( 0.0, 0.0, 0.0, 1.0 );

	if ( u_texActive0 )
	{
		vec4 color0 = texture3D( texture0, v_texcoord );
		color = colormap( color0, u_colormap0, u_lowerThreshold0, u_upperThreshold0, u_selectedMin0, u_selectedMax0, 1.0, color );
	}

	if ( u_texActive1 )
	{
		vec4 color1 = texture3D( texture1, v_texcoord );
		color = colormap( color1, u_colormap1, u_lowerThreshold1, u_upperThreshold1, u_selectedMin1, u_selectedMax1, u_alpha1, color );
	}
	
	if ( u_texActive2 )
	{
		vec4 color2 = texture3D( texture2, v_texcoord );
		color = colormap( color2, u_colormap2, u_lowerThreshold2, u_upperThreshold2, u_selectedMin2, u_selectedMax2, u_alpha2, color );
	}
	
	if ( u_texActive3 )
	{
		vec4 color3 = texture3D( texture3, v_texcoord );
		color = colormap( color3, u_colormap3, u_lowerThreshold3, u_upperThreshold3, u_selectedMin3, u_selectedMax3, u_alpha3, color );
	}
	
	if ( u_texActive4 )
	{
		vec4 color4 = texture3D( texture4, v_texcoord );
		colormap( color4, u_colormap4, u_lowerThreshold4, u_upperThreshold4, u_selectedMin4, u_selectedMax4, u_alpha4, color );
	}

	if ( !( color.r + color.g + color.b > 0.0 ) ) discard; 
	gl_FragColor = color;
}
