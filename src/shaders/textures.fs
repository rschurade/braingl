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

vec4 texColor( vec3 texCoord )
{
    vec4 color0 = vec4( 0.0, 0.0, 0.0, 0.0 );
    vec4 color1 = vec4( 0.0, 0.0, 0.0, 0.0 );
    vec4 color2 = vec4( 0.0, 0.0, 0.0, 0.0 );
    vec4 color3 = vec4( 0.0, 0.0, 0.0, 0.0 );
    vec4 color4 = vec4( 0.0, 0.0, 0.0, 0.0 );

    if ( u_texActive0 )
    {
        color0 = texture( texture0, texCoord );
        if ( u_colormap0 != -1 )
        {
            color0 = colormap( unpackFloat( color0 ), u_colormap0, u_lowerThreshold0, u_upperThreshold0, u_selectedMin0, u_selectedMax0 );
        }
    }

    if ( u_texActive1 )
    {
        color1 = texture( texture1, texCoord );
        if ( u_colormap1 != -1 )
        {
            color1 = colormap( unpackFloat( color1 ), u_colormap1, u_lowerThreshold1, u_upperThreshold1, u_selectedMin1, u_selectedMax1 );
        }
    }
    
    if ( u_texActive2 )
    {
        color2 = texture( texture2, texCoord );
        if ( u_colormap2 != -1 )
        {
            color2 = colormap( unpackFloat( color2 ), u_colormap2, u_lowerThreshold2, u_upperThreshold2, u_selectedMin2, u_selectedMax2 );
        }
    }
    
    if ( u_texActive3 )
    {
        color3 = texture( texture3, texCoord );
        if ( u_colormap3 != -1 )
        {
            color3 = colormap( unpackFloat( color3 ), u_colormap3, u_lowerThreshold3, u_upperThreshold3, u_selectedMin3, u_selectedMax3 );
        }
    }
    
    if ( u_texActive4 )
    {
        color4 = texture( texture4, texCoord );
        if ( u_colormap4 != -1 )
        {
            color4 = colormap( unpackFloat( color4 ), u_colormap4, u_lowerThreshold4, u_upperThreshold4, u_selectedMin4, u_selectedMax4 );
        }
    }
    vec3 mcolor = color0.rgb;
    if( length( color1.rgb ) > 0.0 )
    {
        mcolor = mix( mcolor.rgb, color1.rgb, u_alpha1 );
    }
    if( length( color1.rgb ) > 0.0 )
    {
        mcolor = mix( mcolor.rgb, color2.rgb, u_alpha2 );
    }
    if( length( color1.rgb ) > 0.0 )
    {
        mcolor = mix( mcolor.rgb, color3.rgb, u_alpha3 );
    }
    if( length( color1.rgb ) > 0.0 )
    {
        mcolor = mix( mcolor.rgb, color4.rgb, u_alpha4 );
    }
    return vec4( mcolor, u_alpha0 );
}