#include colormap_fs
#include textures_fs

void main()
{
    vec4 color = texColor( v_texcoord );

	if ( !( color.r + color.g + color.b > 0.0 ) ) discard; 
	
	if ( u_picking )
	{
        gl_FragColor = u_pickColor;
    }
	else
	{
	   gl_FragColor = color;
	}   
	
}
