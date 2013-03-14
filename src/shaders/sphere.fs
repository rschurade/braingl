#version 120 

uniform vec4 u_color;
uniform bool u_picking;

varying vec3 v_normal;

#include lighting_fs

void main( void )
{
    if ( u_picking )
    {
        gl_FragColor = u_color;
    }
    else
    {
        gl_FragColor = blinnPhongIllumination(
            // material properties
            u_color.rgb * 0.2,                    // ambient color
            u_color.rgb,                    // diffuse color
            u_color.rgb,                          // specular color
            1.0,                                  // shininess
        
            // light color properties
            gl_LightSource[0].diffuse.rgb * 0.5,         // light color
            gl_LightSource[0].ambient.rgb * 0.5,         // ambient light
        
            // directions
            normalize( v_normal ),                 // normal
            v_viewDir.xyz,                         // viewdir
            v_lightDir.xyz );                    // light direction
    }
}
