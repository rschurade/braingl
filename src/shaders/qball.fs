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


void main()
{
/*
    gl_FragColor = blinnPhongIllumination(
        // material properties
        gl_Color.rgb * 0.2,                    // ambient color
        gl_Color.rgb * 2.0,                    // diffuse color
        gl_Color.rgb,                          // specular color
        30.0,                                  // shininess

        // light color properties
        gl_LightSource[0].diffuse.rgb,         // light color
        gl_LightSource[0].ambient.rgb,         // ambient light

        // directions
        normalize( v_normal ),                 // normal
        v_viewDir.xyz,                         // viewdir
        v_lightDir.xyz );                    // light direction

*/
    
    gl_FragColor = gl_Color;
}
