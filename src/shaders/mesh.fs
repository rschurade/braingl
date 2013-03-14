#include colormap_fs
#include textures_fs
#include lighting_fs
#include uniforms_fs

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
        gl_FragColor = blinnPhongIllumination(
            // material properties
            gl_Color.rgb * 0.2,                    // ambient color
            gl_Color.rgb,                    // diffuse color
            gl_Color.rgb,                          // specular color
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
