#version 120

#include uniforms_vs
#include peel_vs

////////////////////////////////////////////////////////////////////////////////////////////
// 2: uniforms
/////////////////////////////////////////////////////////////////////////////////////////////
// fractional anisotropy threshold to skip some glyphs
uniform float u_faThreshold;

// eigenvector threshold
uniform float u_evThreshold;

uniform int u_evSelect;

varying float v_discard;

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
    float dir    = a_dir;
    vec3 diag    = a_diag;
    vec3 offdiag = a_offdiag;

    // calculate eigenvectors, and rotation matrix
    vec3 evals = getEigenvalues( diag, offdiag );

    // first eigenvector
    vec3 ABCx = diag - evals.x;
    vec3 ev0 = getEigenvector( ABCx, offdiag );

    // second eigenvector
    vec3 ABCy = diag - evals.y;
    vec3 ev1 = getEigenvector( ABCy, offdiag );

    // third eigenvector
    vec3 ABCz = diag - evals.z;
    vec3 ev2 = cross( ev0.xyz, ev1.xyz ); // as they are orthogonal

    // glyphs color and anisotropy
    float FA = getFA( evals );
    
    float evalSum =    evals.x + evals.y + evals.z;
    
    v_discard = 0.;
    if( evalSum > u_evThreshold )
    {
        v_discard = 1.;
    }
    
    if( FA <= u_faThreshold )
    {
        v_discard = 1.;
    }

    vec4 evtmp = vec4( 0.0 );
    if ( u_evSelect == 1 )
    {
        evtmp = vec4( ev0.rgb, 0.0 ) * dir * u_scaling;
        gl_FrontColor = getColor( ev0.xyz, 1.0 );
    }
    else if ( u_evSelect == 2 )
    {
        evtmp = vec4( ev1.rgb, 0.0 ) * dir * u_scaling;
        gl_FrontColor = getColor( ev1.xyz, 1.0 );
    }
    else if ( u_evSelect == 3 )
    {
        evtmp = vec4( ev2.rgb, 0.0 ) * dir * u_scaling;
        gl_FrontColor = getColor( ev2.xyz, 1.0 );
    }
    v_position = mvp_matrix * ( vec4( a_position, 1.0 ) + evtmp );
    gl_Position = v_position;
}
