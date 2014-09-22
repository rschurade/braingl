#version 330

//simple passthrough but dupe colours to separate and include barycentric coord
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v_vertex[];
out vec3 g_vertex;

in  vec3 v_normal[];
out vec3 g_normal;

in vec3 v_color[];
flat out vec3 g_color[3];

in float  v_value[];
out float g_value[3];

in vec3  v_texCoord0[];
out vec3 g_texCoord0;
in vec3  v_texCoord1[];
out vec3 g_texCoord1;
in vec3  v_texCoord2[];
out vec3 g_texCoord2;
in vec3  v_texCoord3[];
out vec3 g_texCoord3;
in vec3  v_texCoord4[];
out vec3 g_texCoord4;

in vec4  v_viewDir[];
out vec4 g_viewDir;
in vec4  v_lightDir[];
out vec4 g_lightDir;

out vec3 g_coord;

void main()
{
    for ( int i = 0; i < 3; ++i )
    {
        g_color[i] = v_color[i];
        g_value[i] = v_value[i];
    }
    for ( int i = 0; i < 3; ++i )
    {
        g_coord = vec3( 0.0 );
        g_coord[i] = 1.0;
        gl_Position = gl_in[i].gl_Position;
        
        g_vertex = v_vertex[i];
        g_normal = v_normal[i];
        
        
        g_texCoord0 = v_texCoord0[i];
        g_texCoord1 = v_texCoord1[i];
        g_texCoord2 = v_texCoord2[i];
        g_texCoord3 = v_texCoord3[i];
        g_texCoord4 = v_texCoord4[i];
        
        g_viewDir = v_viewDir[i];
        g_lightDir = v_lightDir[i];
        
        EmitVertex();
    }
}