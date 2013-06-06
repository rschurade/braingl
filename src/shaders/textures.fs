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
        color0 = colormap0( color0, u_lowerThreshold0, u_upperThreshold0, u_selectedMin0, u_selectedMax0 );
        if ( length( color0.rgb ) > 0.0 )
        {
            color0.a = u_alpha0;
        }
    }

    if ( u_texActive1 )
    {
        color1 = texture( texture1, texCoord );
        color1 = colormap1( color1, u_lowerThreshold1, u_upperThreshold1, u_selectedMin1, u_selectedMax1 );
        if ( length( color1.rgb ) > 0.0 )
        {
            color1.a = u_alpha1;
        }
    }
    
    if ( u_texActive2 )
    {
        color2 = texture( texture2, texCoord );
        color2 = colormap2( color2, u_lowerThreshold2, u_upperThreshold2, u_selectedMin2, u_selectedMax2 );
        if ( length( color2.rgb ) > 0.0 )
        {
            color2.a = u_alpha2;
        }
    }
    
    if ( u_texActive3 )
    {
        color3 = texture( texture3, texCoord );
        color3 = colormap3( color3, u_lowerThreshold3, u_upperThreshold3, u_selectedMin3, u_selectedMax3 );
        if ( length( color3.rgb ) > 0.0 )
        {
            color3.a = u_alpha3;
        }
    }
    
    if ( u_texActive4 )
    {
        color4 = texture( texture4, texCoord );
        color4 = colormap4( color4, u_lowerThreshold4, u_upperThreshold4, u_selectedMin4, u_selectedMax4 );
        if ( length( color4.rgb ) > 0.0 )
        {
            color4.a = u_alpha4;
        }
    }
    vec3 mcolor = color0.rgb;
    mcolor = mix( mcolor.rgb, color1.rgb, color1.a );
    mcolor = mix( mcolor.rgb, color2.rgb, color2.a );
    mcolor = mix( mcolor.rgb, color3.rgb, color3.a );
    mcolor = mix( mcolor.rgb, color4.rgb, color4.a );
    
    return vec4( mcolor, color0.a );
}