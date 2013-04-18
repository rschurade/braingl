#version 120

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include peel_fs

void main()
{
    vec4 color = texColor( v_texcoord );

	if ( !( color.r + color.g + color.b > 0.0 ) ) discard;
	
	if ( u_renderMode == 5 )
    {
        vec4 d = encode(1.0-gl_FragCoord.z);
        gl_FragColor = d;
        //gl_FragColor = vec4( 1.0-gl_FragCoord.z, 0.0, 0.0, 1.0 );
    } 
    else
    {	
    	if ( u_picking )
    	{
            gl_FragColor = u_pickColor;
        }
    	else
    	{
    	   gl_FragColor = color;
    	}
    }   
}
