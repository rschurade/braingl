#version 120 

varying float v_discard;

void main( void )
{
    if ( v_discard > 0. )
    {
        discard;
    }

    gl_FragColor = vec4( gl_Color.rgb, 1.0 );
}
