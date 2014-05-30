#version 330

#include uniforms_fs

out vec4 fragColor;

void main()
{
	fragColor = frontColor;
}
