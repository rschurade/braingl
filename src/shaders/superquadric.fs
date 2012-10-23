#version 120 

/////////////////////////////////////////////////////////////////////////////////////////////
// 1: varyings
/////////////////////////////////////////////////////////////////////////////////////////////

// light direction
// USAGE:
// x,y,z components:        the light direction vector
// w component:             unused
// (4 varying floats)
varying vec4 v_lightDir;

// camera direction vector
// USAGE:
// x,y,z components:        the direction vector
// w component:             unused
varying vec4 v_planePoint;

// point on projection plane of current pixel
// USAGE:
// x,y,z components:        the point
// w component:             unused
varying vec4 v_viewDir;

// alpha and beta values describing the superquadric
// USAGE:
// x component:             2.0/alpha
// y component:             2.0/beta
// z component:             alpha/beta
// w component:             is !=0 when the glyph has to be dropped
// (4 varying floats)
varying vec4 v_alphaBeta;

/////////////////////////////////////////////////////////////////////////////////////////////
// 2: uniforms
/////////////////////////////////////////////////////////////////////////////////////////////

// scale glyphs
uniform float u_scaling;

// fractional anisotropy threshold to skip some glyphs
uniform float u_faThreshold;

// eigenvector threshold
uniform float u_evThreshold;

// sharpnes parameter
uniform float u_gamma;



/**
 * A struct containing the needed light and material parameters commonly used in most shaders.
 *
 * \note This is for evaluating the phong equation for 1 channel only.
 */
struct wge_LightIntensityParameter
{
    // These 4 parameters are similar to those in gl_MaterialParameters
    float materialAmbient;  //!< Material ambient intensity.
    float materialDiffuse;  //!< Material diffuse intensity.
    float materialSpecular; //!< Material Specular intensity.
    float materialShinines; //!< Material shinines factor

    // These 4 parametes are a stripped down version of gl_LightSourceParameters
    float lightDiffuse;     //!< Light diffuse intensity.
    float lightAmbient;     //!< Light ambient intensity.
    vec3  lightPosition;    //!< Light position in world-space

    vec3  viewDirection;    //!< View direction vector. Well this actually is -vec3( 0.0, 0.0, -1.0 )
};


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

// tollerance value for float comparisons
float zeroTolerance = 0.001;

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- sPow
//
// This function extends the "pow" function with features to handle negative base values.
/////////////////////////////////////////////////////////////////////////////////////////////
float sPow( float x, float y )
{
    if( y == 0.0 )
    {
        return 1.0;
    }
    else
    {
        return pow( abs( x ), y );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- superquadric
//
// This function calculates allmost every value at a given point (the value of the
// superquadric function, gradient and derivate)
//
// Parameters:
// viewDir:     viewing direction vector
// planePoint:  point on projection plane
// t:           current point on ray
// ray:         returns position on ray
// gradient:    the gradient vector at current ray point
// sqd:         value of derived function at current ray point
//
// Returns:
//              The function value of the superquadrics function at current point
/////////////////////////////////////////////////////////////////////////////////////////////
float superquadric( vec3 viewDir, vec3 planePoint, float t, out vec3 ray, out vec3 gradient, out float sqd )
{
    ray = planePoint.xyz + t*viewDir.xyz;

    // those values will be needed multiple times ...
    float rayXYPow = sPow( ray.x, v_alphaBeta.x ) + sPow( ray.y, v_alphaBeta.x );
    float rayZPow = sPow( ray.z, v_alphaBeta.y );

    // the value at given position
    float sq = sPow( rayXYPow, v_alphaBeta.z ) + rayZPow  - 1.0;

    // calculate squadric value for current t

    // if we get a hit we need those values for calculating the gradient at the hit point
    // if we do not get a hit we use those values for calculating the derivative at the current point
    // for doing the next newton step

    /////////////////// HINT ///////////////////
    // If we derive sign(x)*pow(abs(x),y) we use product rule to get:
    //                    sign' * pow + sign * pow'
    // Well the derived sign function is nothing else than the unit impulse delta.
    // It is              delta<>0 iff x=0
    // And also           x=0 -> pow(x, y)=0
    // so delta(x)*pow(x) will allways be 0 ... we can drop it
    //
    // ==> y * sign(x)* sPow(x, y-1.0);


    float a  = sign( rayXYPow ) * sPow( rayXYPow, v_alphaBeta.z - 1.0 );
    float b1 = sign( ray.x ) * sPow( ray.x, v_alphaBeta.x - 1.0 );

    float b2 = sign( ray.y ) * sPow( ray.y, v_alphaBeta.x - 1.0 );
    float c = v_alphaBeta.y  * sign( ray.z ) * sPow( ray.z, v_alphaBeta.y  - 1.0 );

    // now we can reuse some values to calculate the gradient vector at the hit point
    gradient = vec3( v_alphaBeta.y * a * b1, v_alphaBeta.y * a * b2, c );

    // calculate the derived function, reuse as much previously calculated values as possible
    sqd = dot( gradient, viewDir );

    // thats it, return value at current point
    return sq;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- superquadric
//
// This function is the "light" version of the above one. It just calculates the result of
// the superquadrics function.
//
// Parameters:
// viewDir:     viewing direction vector
// planePoint:  point on projection plane
// t:           current point on ray
//
// Returns:
//              The function value of the superquadrics function at current point
/////////////////////////////////////////////////////////////////////////////////////////////
float superquadric( vec3 viewDir, vec3 planePoint, float t )
{
    vec3 ray = planePoint.xyz + t*viewDir.xyz;

    // those values will be needed multiple times ...
    float rayXYPow = sPow( ray.x, v_alphaBeta.x ) + sPow( ray.y, v_alphaBeta.x );
    float rayZPow = sPow( ray.z, v_alphaBeta.y );

    // the value at given position
    float sq = sPow( rayXYPow, v_alphaBeta.z ) + rayZPow  - 1.0;

    // thats it, return value at current point
    return sq;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- main
/////////////////////////////////////////////////////////////////////////////////////////////
void main( void )
{
    // filter out glyphs whose anisotropy is smaller than the threshold or where the eigenvalues
    // are below the threshold (alphaBeta.w is != if this is the case)
    if( v_alphaBeta.w > 0.0 )
    {
        discard;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 1: try to find a goot start value for newton iteration
    /////////////////////////////////////////////////////////////////////////////////////////////

    // well the following value is quite empiric but it works well with squadrics whose beta<=alpha<=1.0
    // HINT: if the ray hits the surface of the glyph lastT will allways be negative!
    float lastT = 0.0;
    int numNewtonLoops = 10;

    // this vector stores the gradient if there has been a hit
    vec3 grad = vec3( 0.0, 0.0, 1.0 );

    // some vars that will be needed later
    // got a hit?
    bool hit = false;
    vec3 hitPoint = vec3( 0.0, 0.0, 0.0 );

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 2: newton iteration to determine roots of the superquadric-ray intersection
    /////////////////////////////////////////////////////////////////////////////////////////////

    // now try to calculate the intersection of the given ray with the superquadric.
    // here we are using newtons iterative method for finding roots

    // the iteration loop (NOTE: due to the fact that the shaders do not support an at compile time unknown loop count we set it to
    // this quite empiric value (which works well (for at least the squadrics with beta<=alpha<=1.0))
    for( int i = 0; i < numNewtonLoops; i++ )
    {
        // calculate all needed values
        float sqd = 0.0;
        float sq = superquadric( v_viewDir.xyz, v_planePoint.xyz, lastT, hitPoint, grad, sqd );

        // new ray parameter
        float newT = lastT - ( sq / sqd );

        // near enough?
        // or has t not changed much since last iteration?
        if( !hit && ( ( abs( sq ) <= zeroTolerance ) || ( abs( newT - lastT ) < zeroTolerance ) ) )
        {
            hit = true;
            break;
        }

        // if the ray parameter starts to jump around (it should get smaller step by step (because lastT is negative))
        // NOTE: this speeds up rendering at rays that will miss the glyph (round about 50% faster!)
        if( newT > lastT )
            break;

        // not near enough -> another newton step
        lastT = newT;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 3: draw or discard the pixel
    /////////////////////////////////////////////////////////////////////////////////////////////

    if( hit )
    {
        // draw shaded pixel
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
        normalize( grad ),                     // normal
        v_viewDir.xyz,                         // viewdir
        v_lightDir.xyz );                      // light direction
    }
    else // no hit: discard
    {
        // want to see the bounding box? uncomment this line
        // gl_FragColor=vec4(0.5, 0.5, 1., 1.0);
        discard;
    }
}