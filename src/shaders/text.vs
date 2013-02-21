attribute vec4 a_position;

uniform float u_width;
uniform float u_height;
uniform float u_x;
uniform float u_y;
uniform float u_size;
uniform float u_pos;
uniform float u_char;

varying vec2 v_texCoord;

void main()
{
    float fontRatio = 0.6;

    float originX = ( ( u_x / u_width ) * 2.0 ) - 1.0;
    float originY = ( ( u_y / u_height ) * 2.0 ) - 1.0;

    float x = ( a_position.x * u_size * fontRatio ) + ( u_pos * u_size * fontRatio ) + originX;
    float y = ( a_position.y * u_size ) + originY;

    gl_Position = vec4( x, y, a_position.z, 1.0 );
        
    float row = 8.0 - floor( u_char / 12.0 );
    float column = u_char - ( row * 12.0 ); 

    float texX = ( 1.0 / 12.0 ) * column + ( 1.0 / 12.0 ) * a_position.x;
    float texY = ( 1.0 / 9.0 ) * row + ( 1.0 / 9.0 ) * a_position.y;  
    
    v_texCoord = vec2( texX, texY );
}
