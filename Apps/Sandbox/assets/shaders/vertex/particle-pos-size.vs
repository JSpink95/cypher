#version 440 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aSize;

out vec3 gsPosition;
out vec2 gsSize;

void main()
{
	gsPosition = aPosition;
	gsSize = aSize;
}