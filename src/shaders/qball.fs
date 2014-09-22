#version 330

in vec4 frontColor;

#include peel_fs

void main()
{
    writePeel( frontColor );
}
