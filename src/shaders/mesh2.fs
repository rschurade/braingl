#version 330

in vec3 g_vertex;
in vec3 g_normal;
flat in vec3 g_color[3];
in float g_value[3];

in vec3 g_coord; //barycentric weights as a result of interp

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

int getVertexId()
{
    return ( g_coord.x > g_coord.y && g_coord.x > g_coord.z ) ? 0 : ( ( g_coord.y > g_coord.z ) ? 1 : 2 );
}

#include colormap_fs
#include meshtextures_fs
#include meshlighting_fs
#include peel_fs


void main()
{
    vec3 vertex = g_vertex;

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
        int vertexId = getVertexId();
        vec4 mcol = vec4( 0.0, 0.0, 0.0, 1.0 );
        mcol = colormap( g_value[vertexId], u_colormap, u_lowerThreshold, u_upperThreshold, u_selectedMin, u_selectedMax );
        color = light( mcol ).rgb;
        
        if ( length( color.rgb ) < 0.00001 )
        {
            color = light( u_color ).rgb;
        } 
    }
    else
    {
        int vertexId = getVertexId();
        color = light( vec4( g_color[vertexId].rgb, 1.0 ) ).rgb;
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
