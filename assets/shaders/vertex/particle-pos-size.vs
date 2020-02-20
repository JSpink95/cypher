#version 440 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aSize;
layout(location = 3) in vec2 aUvScale;
layout(location = 4) in vec2 aUvOffset;

out vec3 gsPosition;
out vec3 gsColor;
out vec2 gsSize;
out vec2 gsUvScale;
out vec2 gsUvOffset;

void main()
{
	gsPosition = aPosition;
	gsColor = aColor;
	gsSize = aSize;
	gsUvScale = aUvScale;
	gsUvOffset = aUvOffset;
}