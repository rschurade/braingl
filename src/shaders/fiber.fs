varying vec3 v_normal;

// light direction
// USAGE:
// x,y,z components:        the light direction vector
// w component:             unused
// (4 varying floats)
varying vec4 v_lightDir;

// point on projection plane of current pixel
// USAGE:
// x,y,z components:        the point
// w component:             unused
varying vec4 v_viewDir;

varying vec3 v_texcoord;
varying float v_extra;

uniform int u_colorMode;
uniform int u_colormap;
uniform float u_selectedMin;
uniform float u_selectedMax;
uniform float u_lowerThreshold;
uniform float u_upperThreshold;
/**
 * Function to calculate lighting based on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 *
 * \param ambient   materials ambient color
 * \param diffuse   materials diffuse color
 * \param specular  materials specular color
 * \param shininess material shininess
 * \param lightColor  the light color
 * \param ambientLight the ambient light color
 * \param normalDir the normal
 * \param viewDir   viewing direction
 * \param lightDir  light direction
 *
 * \return the color.
 */
vec4 blinnPhongIllumination( vec3 ambient, vec3 diffuse, vec3 specular, float shininess,
                             vec3 lightColor, vec3 ambientLight,
                             vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    normalDir *= sign( dot( normalDir, viewDir ) );

    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    vec3 ambientV = ambient * ambientLight;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0.0 );
    vec3 diffuseV = diffuse * diffuseLight;

    // compute specular term
    float specularLight = pow( max( dot( H, normalDir ), 0.0 ), shininess );
    if( diffuseLight <= 0.) specularLight = 0.;
    vec3 specularV = specular * specularLight;

    return vec4( ambientV + ( diffuseV + specularV ) * lightColor, 1.0 );
}

float unpackFloat(const vec4 value) {
    const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    //const vec4 bitSh = vec4(0.0, 0.0, 1.0, 1.0 / 256.0 );
    return (dot(value, bitSh));
}

void main()
{
    if ( u_colorMode == 3 )
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
	    
	    gl_FragColor = color;
    }
    else if ( u_colorMode == 4 )
    {
        vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 );
        gl_FragColor = colormap( v_extra, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax, 1.0, color );
    }
    else
    {
        gl_FragColor = gl_Color;
    }
}
