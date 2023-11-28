#version 330 core

out vec4 outColG;
uniform float uA;
uniform float uR;

void main()
{
	outColG = vec4(uR, uA, uA, 1.0);
}