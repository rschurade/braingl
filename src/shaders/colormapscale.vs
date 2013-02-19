attribute vec4 a_position;


void main()
{
    // Calculate vertex position in screen space
    gl_Position = a_position;
}
