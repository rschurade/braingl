#version 120

attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec3 a_diag;
attribute vec3 a_offdiag;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrixInvert;


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




// (c) 2007 by Mario Hlawitschka

// tensors have to be stored in the following way:
// diag.x = Dxx     Diagonal elements are stored in one vector
// diag.y = Dyy
// diag.z = Dzz
// offdiag.x = Dyz  Off diagonal elements are stored in another vector
// offdiag.y = Dxz      where all components are stored in that location
// offdiag.z = Dxy      that is indexed by the coordinate that is not in the tensor index

// Method similar to the above by Hasan proposed by Cardano et al.
// implementation is more stable than the above one, so use this as
// default in all applications
float sqr( float a )
{
    return a * a;
}

vec3 getEigenvaluesCardano( in vec3 diag, in vec3 offdiag )
{
  const float M_SQRT3 = 1.73205080756887729352744634151;
  float de = offdiag.z * offdiag.x;
  float dd = sqr( offdiag.z );
  float ee = sqr( offdiag.x );
  float ff = sqr( offdiag.y );
  float m  = diag.x + diag.y + diag.z;
  float c1 = diag.x * diag.y + diag.x * diag.z + diag.y * diag.z
             - ( dd + ee + ff );
  float c0 = diag.z * dd + diag.x * ee + diag.y * ff - diag.x * diag.y * diag.z - 2. * offdiag.y * de;

  float p, sqrt_p, q, c, s, phi;
  p = sqr( m ) - 3. * c1;
  q = m * ( p - ( 3. / 2. ) * c1 ) - ( 27. / 2. ) * c0;
  sqrt_p = sqrt( abs( p ) );
  
  

  phi = 27. * ( 0.25 * sqr( c1 ) * ( p - c1 ) + c0 * ( q + 27. / 4. * c0 ) );
  phi = ( 1. / 3. ) * atan( sqrt( abs( phi ) ), q );

  c = sqrt_p * cos( phi );
  s = ( 1. / M_SQRT3 ) * sqrt_p * sin( phi );

  vec3 w;
  w[2] = ( 1. / 3. ) * ( m - c );
  w[1] = w[2] + s;
  w[0] = w[2] + c;
  w[2] -= s;
  return w;
}

vec3 getEigenvalues( in vec3 diag, in vec3 offdiag )
{
  return getEigenvaluesCardano( diag, offdiag );
}

float getMajorEigenvalue( vec3 diag, vec3 offdiag )
{
  return getEigenvalues( diag, offdiag )[0];
}

/**
 * use the above if more than one eigenvector is required and only use this if you need exactly one value
 */
float getMajorEigenvalueOld( vec3 diag, vec3 offdiag )
{
  const float PiThird = 3.14159265358979323846264/3.;
  float I1 = diag.x + diag.y + diag.z;
  float I2 = diag.x * diag.y + diag.x * diag.z + diag.y * diag.z - dot( offdiag, offdiag );
  float I3 = diag.x * diag.y * diag.z + 2. * offdiag.x * offdiag.y * offdiag.z
      - ( diag.x * offdiag.x * offdiag.x + diag.y * offdiag.y * offdiag.y + diag.z * offdiag.z * offdiag.z );

  const float third = 1. / 3.;
  float I1third = I1 * third;
  float I1thirdsqr = I1third * I1third;
  float I2third = I2 * third;
  float v  = I1thirdsqr - I2third;

  vec3 lambda;

  float s  = I1thirdsqr * I1third - I1 * I2 / 6. + 0.5 * I3;

  // for real eigenvalues: v>0, s^2 < v^3
  float sqrtv = sqrt( v );
  float phi= acos( s / ( v * sqrtv ) ) * third;

  float sqrtv2 = 2. * sqrtv;

  // due to the cosine function and the fact, that 0 <= phi <= pi/3
  // it is obvious that the eigenvalues need no further sorting
  return I1third + sqrtv2 * cos( phi );
}

// compute vector direction depending on information computed by getEigenvalues
// before (Hasan et al. 2001)
vec3 getEigenvector( vec3 ABC /*diag without eigenalue i*/, vec3 offdiag )
{
  vec3 vec;
  vec.x = ( offdiag.z * offdiag.x - ABC.y * offdiag.y ) * ( offdiag.y * offdiag.x - ABC.z * offdiag.z ); // FIXME
  //< last component is missing in the paper! there is only a Dx?
  vec.y = ( offdiag.y * offdiag.x - ABC.z * offdiag.z ) * ( offdiag.y * offdiag.z - ABC.x * offdiag.x );
  vec.z = ( offdiag.z * offdiag.x - ABC.y * offdiag.y ) * ( offdiag.y * offdiag.z - ABC.x * offdiag.x );

  return normalize( vec );
}

// FA as used everywhere (Kondratieva et al 2005)
// computes FA from eigenvalues
float getFA( vec3 evalues )
{
  float sqrt32 = sqrt( 3. / 2. ); // would make this const, if compiler let me
  float I1 = evalues.x + evalues.y + evalues.z;
  float mu = I1 / 3.;
  vec3 deriv;
  deriv.x = ( evalues.x - mu );
  deriv.y = ( evalues.y - mu );
  deriv.z = ( evalues.z - mu );
  float FA = sqrt32 * length( deriv ) / length( evalues );
  return FA;
}

// Compute FA without doing eigenvalue decomposition
float getFA( vec3 diag, vec3 offdiag )
{
  float cross = dot( offdiag, offdiag );
  float j2 = diag.x * diag.y + diag.y * diag.z + diag.z * diag.x - cross;
  float j4 = dot( diag, diag ) + 2. * cross;
  return sqrt( ( j4 - j2 ) / j4 );
}


// simple default color map, but with precomputed FA values
vec4 getColor( vec3 dir, float FA )
{
  vec4 color;
  //dir = normalize( dir );
  //dir = clamp( dir, 1., 1.);
  color = vec4( abs( dir ) * FA, 1. );
  return color;
}

// compute the default RGB color scheme
vec4 getColor( vec3 evec, vec3 evalues )
{
  float fa = getFA( evalues );
  return getColor( evec, fa );
}

// make things transparent, proportional to FA
vec4 getColorAlpha( vec3 evec, vec3 evalues )
{
  float fa = getFA( evalues );
  //evec = clamp( evec, 1., 1.);
  return vec4( abs( evec ), 1. ) * fa;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- vertex shader -- main
//
/////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    // 1: standart shader stuff
    /////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 2: aquire tensor data, calculate eigen system
    /////////////////////////////////////////////////////////////////////////////////////////////

    // get tensor data
    vec4 normal  = a_normal;
    vec3 diag    = a_diag;
    vec3 offdiag = a_offdiag;

    // calculate eigenvectors, and rotation matrix
    vec3 evals = getEigenvalues( diag, offdiag );
    //evals = vec3( 1.0, 1.0, 1.0 );

    // first eigenvector
    vec3 ABCx = diag - evals.x;
    vec3 ev0 = getEigenvector( ABCx, offdiag );
    //ev0 = normalize( vec3( 1.0, 0.0, 0.0 ) );

    // second eigenvector
    vec3 ABCy = diag - evals.y;
    vec3 ev1 = getEigenvector( ABCy, offdiag );
    //ev1 = normalize( vec3( 0.0, 1.0, 0.0 ) );

    // third eigenvector
    vec3 ABCz = diag - evals.z;
    //vec3 ev2 = getEigenvector( ABCz, offdiag );
    vec3 ev2 = cross( ev0.xyz, ev1.xyz ); // as they are orthogonal

    // glyphs color and anisotropy
    float FA = getFA( evals );
    FA = clamp( FA, 0.0, 1.000 ); // filter out invalid FA values later
    gl_FrontColor = getColor( ev0.xyz, FA );

    // throw away glyphs whose FA is below threshold and whose eigenvalues are below threshold
    v_alphaBeta.w = 0.0;
    if( FA <= u_faThreshold )
    {
        v_alphaBeta.w = 1.0;
    }
    if( evals.z <= 0.0 )
    {
        v_alphaBeta.w = 1.0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 3: precalculate some glyph characteristics
    /////////////////////////////////////////////////////////////////////////////////////////////

    // calculate alpha and beta corresponding to eigenvalues
    float evalSum =    evals.x + evals.y + evals.z;
    float cl =       ( evals.x - evals.y ) / evalSum;
    float cp = 2.0 * ( evals.y - evals.z ) / evalSum;
    float cs = 3.0 * ( evals.z )           / evalSum;

    // by default we use ellipsoids
    float kmAlpha = 1.0;
    float kmBeta = 1.0;

    // the above values define the shape of the glyph
    if( cl >= cp )
    {
        kmAlpha = pow( 1.0 - cp, u_gamma );
        kmBeta  = pow( 1.0 - cl, u_gamma );
    }
    else
    {
        kmAlpha = pow( 1.0 - cl, u_gamma );
        kmBeta  = pow( 1.0 - cp, u_gamma );
    }

    // limit the alpha/beta range to [0.2,1.0]
    // HINT: we do this to avoid the exploding number of needed iterations for smaller alpha/beta to find the hit point
    kmAlpha = clamp( kmAlpha, 0.2, 1.0 );
    kmBeta  = clamp( kmBeta, 0.2, 1.0 );

    // finally do some precalculations on alpha and beta
    v_alphaBeta.x = 2.0 / kmAlpha;
    v_alphaBeta.y = 2.0 / kmBeta;
    v_alphaBeta.z = kmAlpha / kmBeta;

    evals.z = ( evals.z / evals.x );
    evals.y = ( evals.y / evals.x );
    evals.x = 1.0;
//    if( evals.z <= u_evThreshold )
    //{
        //v_alphaBeta.w = 1.0;
    //}

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 4: now span the quad according to bounding box, so that the fragment shader can render
    //    the correctly transformed superquadric without clipped edges etc.
    /////////////////////////////////////////////////////////////////////////////////////////////

    // rotation-matrix describing the coordinate system rotation corresponding to the eigenvectors
    mat4 glyphSystem = mat4( ev0.xyz, 0.0,
                             ev1.xyz, 0.0,
                             ev2.xyz, 0.0,
                             0.0, 0.0, 0.0, 1.0 );

    float dimX = u_scaling * evals.x;
    float dimY = u_scaling * evals.y;
    float dimZ = u_scaling * evals.z;

    // calculate propper bounding volume for this quadric (scale)
    mat4 glyphScale = mat4( dimX, 0.0, 0.0, 0.0,
                            0.0, dimY, 0.0, 0.0,
                            0.0, 0.0, dimZ, 0.0,
                            0.0, 0.0, 0.0,  1.0 );
    mat4 glyphScaleInverse = mat4( 1.0 / dimX, 0.0, 0.0, 0.0,
                                   0.0, 1.0 / dimY, 0.0, 0.0,
                                   0.0, 0.0, 1.0 / dimZ, 0.0,
                                   0.0, 0.0, 0.0, 1.0 );

    normal.w = 0.0;
    gl_Position = mvp_matrix * ( a_position + glyphSystem * glyphScale * normal );

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 5: Transform light and plane as well as ray back to glyph space
    /////////////////////////////////////////////////////////////////////////////////////////////

    mat4 glyphToWorld = glyphScaleInverse * transpose( glyphSystem ) * mv_matrixInvert;

    // calculate light direction once per quadric
    vec4 lightPos = vec4( 0.0, 0.0, 1.0, 0.0 );
    
    v_lightDir.xyz = normalize( ( glyphToWorld * lightPos ).xyz );

    // the viewing direction for this vertex:
    v_viewDir = ( glyphToWorld * vec4( 0.0, 0.0, 1.0, 0.0 ) );
    v_viewDir.w = 1.0;

    v_planePoint.xyz = normal.xyz;
}
