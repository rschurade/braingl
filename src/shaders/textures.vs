uniform vec3 u_dims0;
uniform vec3 u_dims1;
uniform vec3 u_dims2;
uniform vec3 u_dims3;
uniform vec3 u_dims4;

uniform vec3 u_adjust0;
uniform vec3 u_adjust1;
uniform vec3 u_adjust2;
uniform vec3 u_adjust3;
uniform vec3 u_adjust4;

out vec3 v_texCoord0;
out vec3 v_texCoord1;
out vec3 v_texCoord2;
out vec3 v_texCoord3;
out vec3 v_texCoord4;

void calcTexCoords()
{
    v_texCoord0 = vec3( a_position.x / u_dims0.x - u_adjust0.x, a_position.y / u_dims0.y - u_adjust0.y, a_position.z / u_dims0.z - u_adjust0.z );
    v_texCoord1 = vec3( a_position.x / u_dims1.x - u_adjust1.x, a_position.y / u_dims1.y - u_adjust1.y, a_position.z / u_dims1.z - u_adjust1.z );
    v_texCoord2 = vec3( a_position.x / u_dims2.x - u_adjust2.x, a_position.y / u_dims2.y - u_adjust2.y, a_position.z / u_dims2.z - u_adjust2.z );
    v_texCoord3 = vec3( a_position.x / u_dims3.x - u_adjust3.x, a_position.y / u_dims3.y - u_adjust3.y, a_position.z / u_dims3.z - u_adjust3.z );
    v_texCoord4 = vec3( a_position.x / u_dims4.x - u_adjust4.x, a_position.y / u_dims4.y - u_adjust4.y, a_position.z / u_dims4.z - u_adjust4.z );
}