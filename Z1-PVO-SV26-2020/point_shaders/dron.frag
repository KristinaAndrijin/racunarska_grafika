#version 330 core

out vec4 outCol;
uniform float uA;

void main()
{
	outCol = vec4(uA, uA, 1.0, 1.0);
}