in vec4 g_lightDir;
in vec4 g_viewDir;

uniform bool u_lighting;

uniform float u_lightAmbient;
uniform float u_lightDiffuse;

uniform float u_materialAmbient;
uniform float u_materialDiffuse;
uniform float u_materialSpecular;
uniform float u_materialShininess;

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
    if( diffuseLight <= 0. ) 
    {
        specularLight = 0.;
    }
    vec3 specularV = specular * specularLight;

    return vec4( ambientV + ( diffuseV + specularV ) * lightColor, 1.0 );
}

vec4 light( vec4 color )
{
    if( !u_lighting )
    {
        return color;
    }

    vec3 diffuse = vec3( 1.0, 1.0, 1.0 );
    vec3 ambient = vec3( 1.0, 1.0, 1.0 );

    return blinnPhongIllumination(
            // material properties
            color.rgb * u_materialAmbient,                    // ambient color
            color.rgb * u_materialDiffuse,                    // diffuse color
            color.rgb * u_materialSpecular,                   // specular color
            u_materialShininess,                              // shininess
        
            // light color properties
            diffuse * u_lightDiffuse,         // light color
            ambient * u_lightAmbient,         // ambient light
        
            // directions
            normalize( g_normal ),               // normal
            g_viewDir.xyz,                       // viewdir
            g_lightDir.xyz );                    // light direction
}
