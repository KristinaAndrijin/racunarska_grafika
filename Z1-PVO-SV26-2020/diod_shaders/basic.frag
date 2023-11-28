#version 330 core

out vec4 outCol;
uniform vec4 triangleColor;

void main()
{
	outCol = triangleColor;
}