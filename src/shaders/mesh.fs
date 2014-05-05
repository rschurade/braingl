#version 330

#include colormap_fs
#include textures_fs
#include uniforms_fs
#include lighting_fs
#include peel_fs

in float v_value;
in vec3 vertex;

void main()
{
    bool dis = false;
    if ( u_cutLowerX && vertex.x < u_x + 0.1 )
    {
        if ( u_cutLowerY && vertex.y < u_y + 0.1 )
        {
            if ( u_cutLowerZ && vertex.z < u_z + 0.1 )
            {
                dis = true;
            }
            if ( u_cutHigherZ && vertex.z > u_z - 0.1)
            {
                dis = true;
            }
        }
        if ( u_cutHigherY && vertex.y > u_y - 0.1 )
        {
            if ( u_cutLowerZ && vertex.z < u_z + 0.1 )
            {
                dis = true;
            }
            if ( u_cutHigherZ && vertex.z > u_z - 0.1 )
            {
                dis = true;
            }
        }
    }   
    if ( u_cutHigherX && vertex.x > u_x - 0.1 )
    {
        if ( u_cutLowerY && vertex.y < u_y + 0.1 )
        {
            if ( u_cutLowerZ && vertex.z < u_z + 0.1 )
            {
                dis = true;
            }
            if ( u_cutHigherZ && vertex.z > u_z - 0.1 )
            {
                dis = true;
            }
        }
        if ( u_cutHigherY && vertex.y > u_y - 0.1 )
        {
            if ( u_cutLowerZ && vertex.z < u_z + 0.1 )
            {
                dis = true;
            }
            if ( u_cutHigherZ && vertex.z > u_z - 0.1 )
            {
                dis = true;
            }
        }
    }   

    if ( dis )
    {
        discard;
    }


    vec3 color = light( u_color ).rgb;
    float test1 = dot( v_lightDir.xyz, v_normal.xyz );
       
    if ( u_colorMode == 0 )
    {
        color = light( u_color ).rgb;
    }
    else if ( u_colorMode == 1 )
    {
        vec4 tcol = texColor();
        
        color = light( tcol ).rgb;
        
        if ( length( color.rgb ) < 0.00001 )
        {
            color = light( u_color ).rgb;
        }
    }
    else if ( u_colorMode == 3 )
    {
        vec4 mcol = vec4( 0.0, 0.0, 0.0, 1.0 );
        mcol = colormap( v_value, u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
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
