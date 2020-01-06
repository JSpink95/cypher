#version 440 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aSize;
layout(location = 1) in vec2 aUvScale;
layout(location = 1) in vec2 aUvOffset;

out vec3 gsPosition;
out vec2 gsSize;
out vec2 gsUvScale;
out vec2 gsUvOffset;

void main()
{
	gsPosition = aPosition;
	gsSize = aSize;
	gsUvScale = aUvScale;
	gsUvOffset = aUvOffset;
}