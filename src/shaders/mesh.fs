#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include lighting_fs
#include peel_fs

in float v_value;

void main()
{
    vec3 color = light( u_color ).rgb;
    float test1 = dot( v_lightDir.xyz, v_normal.xyz );
       
    if ( u_colorMode == 0 )
    {
        if ( test1 > -0.3 && test1 < 0.3 )
        {
            color = vec3( 0.0 );
        }
        else
        {
            color = light( u_color ).rgb;
        }
    }
    else if ( u_colorMode == 1 )
    {
        vec4 tcol = texColor( v_texcoord );
        
        color = light( tcol ).rgb;
        
        if ( length( color.rgb ) < 0.00001 )
        {
            color = light( u_color ).rgb;
        }
    }
    else if ( u_colorMode == 3 )
    {
        vec4 mcol = vec4( 0.0, 0.0, 0.0, 1.0 );
        mcol = colormap( v_value, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax, 1.0, mcol );
        color = light( mcol ).rgb;
        
        if ( length( color.rgb ) < 0.00001 )
        {
            color = light( u_color ).rgb;
        } 
    }
    else
    {
        color = light( frontColor ).rgb;
    }
    
    writePeel( color );
}
