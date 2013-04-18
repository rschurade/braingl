uniform vec4 u_color;

#include peel_fs



void main()
{
    writePeel( gl_Color.rgb );
}
