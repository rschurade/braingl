#version 330

//simple passthrough but dupe colours to separate and include barycentric coord
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v_vertex[];
out vec3 g_vertex[3];

in  vec3 v_normal[];
out vec3 g_normal[3];

in vec3 v_color[];
flat out vec3 g_color[3];

in float  v_value[];
out float g_value[3];

in vec3  v_texCoord0[];
out vec3 g_texCoord0[3];
in vec3  v_texCoord1[];
out vec3 g_texCoord1[3];
in vec3  v_texCoord2[];
out vec3 g_texCoord2[3];
in vec3  v_texCoord3[];
out vec3 g_texCoord3[3];
in vec3  v_texCoord4[];
out vec3 g_texCoord4[3];

in vec4  v_viewDir[];
out vec4 g_viewDir[3];
in vec4  v_lightDir[];
out vec4 g_lightDir[3];

out vec3 g_coord;

void main()
{
    for ( int i = 0; i < 3; ++i )
    {
        g_color[i] = v_color[i];
        g_vertex[i] = v_vertex[i];
        g_normal[i] = v_normal[i];
        g_value[i] = v_value[i];
        
        g_texCoord0[i] = v_texCoord0[i];
        g_texCoord1[i] = v_texCoord1[i];
        g_texCoord2[i] = v_texCoord2[i];
        g_texCoord3[i] = v_texCoord3[i];
        g_texCoord4[i] = v_texCoord4[i];
        
        g_viewDir[i] = v_viewDir[i];
        g_lightDir[i] = v_lightDir[i];
    }
    for ( int i = 0; i < 3; ++i )
    {
        g_coord = vec3( 0.0 );
        g_coord[i] = 1.0;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
}