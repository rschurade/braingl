uniform vec2 u_canvasSize;
uniform int u_renderMode;
uniform float u_alpha;
uniform vec4 u_color;
uniform vec4 u_pickColor;

uniform sampler2D D0; // TEXTURE3 - opaque depth map (uMinorMode 5)
uniform sampler2D D1; // TEXTURE4 - 1st of two ping-pong depth maps (uMinorMode 6)
uniform sampler2D D2; // TEXTURE5 - 2nd of two ping-pong depth maps (uMinorMode 7); also used for C4
uniform sampler2D P0; // TEXTURE5 - 2nd of two ping-pong depth maps (uMinorMode 7); also used for C4
uniform sampler2D C5; // 

in vec4 v_position;

layout( location = 0 ) out vec4 fragColor;
layout( location = 1 ) out vec4 fragDepth;
layout( location = 2 ) out vec4 pickColor;

vec4 encode( float k ) 
{ // assumes k is >= 0
    if ( k <= 0.0 ) return vec4( 0.0, 0.0, 0.0, 0.0 );
    return vec4(
        floor( 256.0 * k) / 255.0,
        floor( 256.0 * fract( 256.0 * k ) ) / 255.0,
        floor( 256.0 * fract( 65536.0 * k ) ) / 255.0,
        0.0);
}

float decode( vec4 d ) 
{
    if ( length( d ) == 0.0 ) return 0.0;
    return ( 65536.0 * d[0] + 256.0 * d[1] + d[2] ) / 16777216.0;
}

void writePeel( vec4 color )
{
    // opaque
    vec4 c = encode( 1.0 - gl_FragCoord.z );
    float z = decode(c);
    float zmin;
    float zmax;
    if ( u_renderMode == 1 || u_renderMode == 0 )
    { 
        fragColor = color;
        fragDepth = c;
        pickColor = u_pickColor;
    }
    else
    {
        vec2 loc = vec2( gl_FragCoord.x/u_canvasSize.x, gl_FragCoord.y/u_canvasSize.y );
        z = decode( encode( 1.0 - gl_FragCoord.z ) ); // bigger number => closer to camera; distance out of screen
        zmin = decode( texture( D0, loc ) );
        if ( u_renderMode == 2 )
        {
            //first creation of D1
            if ( z > zmin ) 
            {
                fragColor = color;
                fragDepth = c;
                pickColor = u_pickColor;
            } 
            else 
            {
                discard;
            }
            
        }
        else if ( u_renderMode == 3 )
        {
            zmax = decode( texture( D1, loc ) );
            if ( zmin < z && z < zmax )
            {
                fragColor = color;
                fragDepth = c;
                vec4 pc = texture( P0, loc );
                pickColor = pc;
            } 
            else 
            {
                discard;
            }
        }
        else if ( u_renderMode == 4 )
        {
            zmax = decode( texture( D2, loc ) );
            if ( zmin < z && z < zmax ) 
            {
                fragColor = color;
                fragDepth = c;
                vec4 pc = texture( P0, loc );
                pickColor = pc;
            } 
            else 
            {
                discard;
            }
        }
        else if ( u_renderMode == 5 )
        {
            zmax = decode( texture( D2, loc ) );
            if ( zmin < z && z < zmax ) 
            {
                vec4 accuColor = texture( C5, loc );
                
                fragColor = vec4( accuColor.r + color.r * color.a, accuColor.g + color.g * color.a, accuColor.b + color.b * color.a, accuColor.a + color.a );
                float count = decode( texture( D1, loc ) );
                count += 1.0;
                fragDepth = encode( count );
                
                vec4 pc = texture( P0, loc );
                pickColor = pc;
            } 
            else 
            {
                discard;
            }
        }
    }
}

void writePeel2( vec4 color )
{
    // opaque
    vec4 c = encode( 1.0 - gl_FragCoord.z );
    float z = decode(c);
    float zmin;
    float zmax;
    if ( u_renderMode == 1 || u_renderMode == 0 )
    { 
        fragColor = color;
        fragDepth = c;
        pickColor = u_pickColor;
    }
    else
    {
        vec2 loc = vec2( gl_FragCoord.x/u_canvasSize.x, gl_FragCoord.y/u_canvasSize.y );
        z = decode( encode( 1.0 - gl_FragCoord.z ) ); // bigger number => closer to camera; distance out of screen
        zmin = decode( texture( D0, loc ) );
        if ( u_renderMode == 2 )
        {
            //first creation of D1
            if ( z > zmin ) 
            {
                fragColor = color;
                fragDepth = c;
                pickColor = u_pickColor;
            } 
            else 
            {
                discard;
            }
            
        }
        else if ( u_renderMode == 3 )
        {
            zmax = decode( texture( D1, loc ) );
            if ( zmin < z && z < zmax )
            {
                fragColor = vec4( 0.0 ); //color;
                fragDepth = c;
                vec4 pc = texture( P0, loc );
                pickColor = pc;
            } 
            else 
            {
                discard;
            }
        }
        else if ( u_renderMode == 4 )
        {
            zmax = decode( texture( D2, loc ) );
            if ( zmin < z && z < zmax ) 
            {
                fragColor =  vec4( 0.0 ); //color;
                fragDepth = c;
                vec4 pc = texture( P0, loc );
                pickColor = pc;
            } 
            else 
            {
                discard;
            }
        }
        else if ( u_renderMode == 5 )
        {
            zmax = decode( texture( D2, loc ) );
            if ( zmin < z && z < zmax ) 
            {
                vec4 accuColor = texture( C5, loc );
                
                fragColor = vec4( accuColor.r + color.r * color.a, accuColor.g + color.g * color.a, accuColor.b + color.b * color.a, accuColor.a + color.a );
                float count = decode( texture( D1, loc ) );
                count += 1.0;
                fragDepth = encode( count );
                
                vec4 pc = texture( P0, loc );
                pickColor = pc;
            } 
            else 
            {
                discard;
            }
        }
    }
}
