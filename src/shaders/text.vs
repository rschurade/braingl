#version 330

#include uniforms_vs
#include peel_vs

out vec2 v_texCoord;

void main()
{
    float fontRatio = 0.5;

    float originX = ( ( u_x / u_width ) * 2.0  * ( u_width  / 1000 ) ) - 1.0;
    float originY = ( ( u_y / u_height ) * 2.0 * ( u_height / 1000 ) ) - 1.0;

    float x = ( a_position.x * u_sizeX * fontRatio * ( u_width / 1000 ) ) + ( u_pos * u_sizeX * fontRatio * ( u_width  / 1000 ) ) + originX;
    float y = ( a_position.y * u_sizeY * ( u_height / 1000 ) ) + originY;
    v_position = vec4( x, y, a_position.z, 1.0 );
    gl_Position = v_position; 
        
    float row = 8.0 - floor( u_char / 12.0 );
    float column = u_char - ( row * 12.0 ); 

    float texX = ( 1.0 / 12.0 ) * column + ( 1.0 / 12.0 ) * a_position.x;
    float texY = ( 1.0 / 9.0 ) * row + ( 1.0 / 9.0 ) * a_position.y;  
    
    v_texCoord = vec2( texX, texY );
}
