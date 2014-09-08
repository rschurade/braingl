#version 330

in vec3 vertex;
in vec3 v_normal;
in vec4 v_color;
in float v_value;

uniform int u_colorMode;
uniform int u_colormap;
uniform float u_min;
uniform float u_max;
uniform float u_selectedMin;
uniform float u_selectedMax;
uniform float u_lowerThreshold;
uniform float u_upperThreshold;

uniform float u_x;
uniform float u_y;
uniform float u_z;

uniform bool u_cutLowerX;
uniform bool u_cutLowerY;
uniform bool u_cutLowerZ;
uniform bool u_cutHigherX;
uniform bool u_cutHigherY;
uniform bool u_cutHigherZ;

uniform int u_meshTransparency;

#include colormap_fs
#include textures_fs
#include lighting_fs
#include peel_fs

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
       
    if ( u_colorMode == 0 )
    {
        color = light( u_color ).rgb;
    }
    else if ( u_colorMode == 1 )
    {
        color = light( texColor() ).rgb;
        
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
        color = light( v_color ).rgb;
    }
    
    if ( u_meshTransparency == 0 )
    {
        writePeel( vec4( color, u_alpha ) );
    }
    else
    {
        writePeel2( vec4( color, u_alpha ) );
    }
}
