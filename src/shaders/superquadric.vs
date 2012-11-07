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
// offdiag.x = Dxy      Off diagonal elements are stored in another vector
// offdiag.y = Dxz      where all components are stored in that location
// offdiag.z = Dyz      that is indexed by the coordinate that is not in the tensor index

float sqr( float a )
{
    return a * a;
}

float sqr3( float a )
{
    return a * a * a;
}

vec3 getEigenvaluesAlfredo( in vec3 diag, in vec3 offdiag )
{
    float M_PI = 3.14159265359;
    float xx = diag.x;
    float xy = offdiag.x;
    float xz = offdiag.y;
    float yy = diag.y;
    float yz = offdiag.z;
    float zz = diag.z;

    float i1 = xx + yy + zz;
    float i2 = xx*yy + xx*zz + yy*zz - ( sqr(xy) + sqr(xz) + sqr(yz) );
    float i3 = xx*yy*zz+ 2.*xy*xz*yz - ( zz*sqr(xy) + yy*sqr(xz) + xx*sqr(yz));

    float v = sqr(i1/3)-i2/3;
    float s = sqr3(i1/3)-i1*i2/6+i3/2;
    
    float phi = 0;
    float l1 = 0;
    float l2 = 0;
    float l3 = 0;
    
    if ((v > 0) && (sqr(s) < sqr3(v)))
      phi = acos( s/v * sqrt( 1./v ) ) /3;
    else phi = 0;

    // eigenvalues
    if (phi !=0) {
      l1 = i1/3+2 * sqrt(v) * cos(phi);
      l2 = i1/3-2 * sqrt(v) * cos(M_PI/3. + phi);
      l3 = i1/3-2 * sqrt(v) * cos(M_PI/3. - phi);
    }
    else
      l1=l2=l3=0.0;

    vec3 w;
    w.x = l1;
    w.y = l2;
    w.z = l3;
    return w;
}

vec3 getEigenvalues( in vec3 diag, in vec3 offdiag )
{
    return getEigenvaluesAlfredo( diag, offdiag );
}

float getMajorEigenvalue( vec3 diag, vec3 offdiag )
{
    return getEigenvalues( diag, offdiag )[0];
}

// compute vector direction depending on information computed by getEigenvalues
// before (Hasan et al. 2001)
vec3 getEigenvector( vec3 ABC /*diag without eigenalue i*/, vec3 offdiag )
{
    vec3 vec;
    vec.x = ( offdiag.x * offdiag.z - ABC.y * offdiag.y ) * ( offdiag.y * offdiag.z - ABC.z * offdiag.x );
    vec.y = ( offdiag.y * offdiag.z - ABC.z * offdiag.x ) * ( offdiag.y * offdiag.x - ABC.x * offdiag.z );
    vec.z = ( offdiag.x * offdiag.z - ABC.y * offdiag.y ) * ( offdiag.y * offdiag.x - ABC.x * offdiag.z );

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
        evals.z = 0.01;
        //v_alphaBeta.w = 1.0;
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

    //evals.z = ( evals.z / evals.x );
    //evals.y = ( evals.y / evals.x );
    //evals.x = 1.0;
    if( evalSum > u_evThreshold )
    {
        v_alphaBeta.w = 1.0;
    }

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
