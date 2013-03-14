#include colormap_fs
#include textures_fs
#include uniforms_fs
#include lighting_fs


void main()
{
    if ( u_colorMode == 1 )
    {
        vec4 color = texColor( v_texcoord );
    
        if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
        
        gl_FragColor = color;
    }
    else
    {
        light();
    }    
}
