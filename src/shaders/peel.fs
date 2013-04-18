uniform vec2 u_canvasSize;
uniform int u_renderMode;
uniform float u_alpha;

varying vec4 v_position;

uniform sampler2D D0; // TEXTURE3 - opaque depth map (uMinorMode 5)
uniform sampler2D D1; // TEXTURE4 - 1st of two ping-pong depth maps (uMinorMode 6)
uniform sampler2D D2; // TEXTURE5 - 2nd of two ping-pong depth maps (uMinorMode 7); also used for C4

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

void writePeel( vec3 color )
{
    // picking
    if ( u_renderMode == 1 )
    {
        gl_FragColor = u_color;
    }
    else 
    {
        // opaque
        if ( u_renderMode == 4 )
        { 
            gl_FragColor = vec4( color, 1.0 );
        }
        else if ( u_renderMode > 7 )
        {
            vec2 loc = vec2( gl_FragCoord.x/u_canvasSize.x, gl_FragCoord.y/u_canvasSize.y );
            float z = decode( encode( 1.0 - gl_FragCoord.z ) ); // bigger number => closer to camera; distance out of screen
            float zmin = decode( texture2D( D0, loc ) );
            float zmax;
            if ( u_renderMode == 9 ) 
            { // C1
                if (z > zmin) 
                {
                    gl_FragColor = vec4( color, u_alpha );
                } 
                else 
                {
                    discard;
                }
            } 
            else 
            {
                if ( u_renderMode == 11 ) 
                { 
                    // C3 (11)
                    zmax = decode( texture2D( D2, loc ) );
                } 
                else 
                { 
                    // C2 (10) or C4 (12)
                    zmax = decode( texture2D( D1, loc ) );
                }
                if ( zmin < z && z < zmax ) 
                {
                    gl_FragColor = vec4( color, u_alpha );
                } 
                else 
                {
                    discard;
                }
            }
        }
        else
        {
            // create depth map, D0 (5); ping-pong D1 (6) and D2 (7)
            vec4 c = encode(1.0-gl_FragCoord.z);
            float z = decode(c);
            if (u_renderMode == 5) 
            { // D0
                gl_FragColor = c;
            } 
            else 
            {
                vec2 loc = vec2( gl_FragCoord.x/u_canvasSize.x, gl_FragCoord.y/u_canvasSize.y );
                float zmin = decode( texture2D( D0, loc ) );
                float zmax;
                if ( u_renderMode == 6 ) 
                { // first creation of D1
                    if (z > zmin) 
                    {
                        gl_FragColor = c;
                    } else 
                    {
                        discard;
                    }
                } 
                else 
                {
                    if (u_renderMode == 7) 
                    {
                        zmax = decode(texture2D(D1, loc)); // create D2
                    } 
                    else 
                    {
                        zmax = decode(texture2D(D2, loc)); // D1b; create D1 again
                    }
                    if (zmin < z && z < zmax) 
                    {
                        gl_FragColor = c;
                    } 
                    else 
                    {
                        discard; // All pixels are discarded, since (zmax < z && z < zmax) is always false
                    }
                }
            }
        }
    }
}