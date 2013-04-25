#include uniforms_fs
#include peel_fs

varying float v_discard;

void main()
{ 
    if ( v_discard > 0.0 )
    {
        discard;
    }
    writePeel( gl_Color.rgb );
}
