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

float unpackFloat(const vec4 value) 
{
    const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    return (dot(value, bitSh));
}

vec4 texColor( vec3 texCoord )
{
    vec4 color;
    color = vec4( 0.0, 0.0, 0.0, 1.0 );

    if ( u_texActive0 )
    {
        vec4 color0 = texture( texture0, texCoord );
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
        vec4 color1 = texture( texture1, texCoord );
        if ( u_colormap1 == -1 )
        {
            if ( ( color1.r + color1.g + color1.b > 0.0 ) )
            {
                color = mix( color, color1, u_alpha1 );
            }
        }
        else
        { 
            color = colormap( unpackFloat( color1 ), u_colormap1, u_lowerThreshold1, u_upperThreshold1, u_selectedMin1, u_selectedMax1, u_alpha1, color );
        }
    }
    
    if ( u_texActive2 )
    {
        vec4 color2 = texture( texture2, texCoord );
        if ( u_colormap2 == -1 )
        {
            if ( ( color2.r + color2.g + color2.b > 0.0 ) )
            {
                color = mix( color, color2, u_alpha2 );
            }
        }
        else
        { 
            color = colormap( unpackFloat( color2 ), u_colormap2, u_lowerThreshold2, u_upperThreshold2, u_selectedMin2, u_selectedMax2, u_alpha2, color );
        }
    }
    
    if ( u_texActive3 )
    {
        vec4 color3 = texture( texture3, texCoord );
        if ( u_colormap4 == -1 )
        {
            if ( ( color3.r + color3.g + color3.b > 0.0 ) )
            {
                color = mix( color, color3, u_alpha3 );
            }
        }
        else
        { 
            color = colormap( unpackFloat( color3 ), u_colormap3, u_lowerThreshold3, u_upperThreshold3, u_selectedMin3, u_selectedMax3, u_alpha3, color );
        }
    }
    
    if ( u_texActive4 )
    {
        vec4 color4 = texture( texture4, texCoord );
        if ( u_colormap4 == -1 )
        {
            if ( ( color4.r + color4.g + color4.b > 0.0 ) )
            {
                color = mix( color, color4, u_alpha4 );
            }
        }
        else
        { 
            colormap( unpackFloat( color4 ), u_colormap4, u_lowerThreshold4, u_upperThreshold4, u_selectedMin4, u_selectedMax4, u_alpha4, color );
        }
    }
    return color;
}