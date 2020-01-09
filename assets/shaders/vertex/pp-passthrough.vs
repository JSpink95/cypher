#version 440 core

layout(location = 0) in vec2 aPosition;

out vec2 fTexcoord;

void main()
{
	gl_Position = vec4(aPosition, 0.0, 1.0);
	fTexcoord = aPosition * 0.5 + 0.5;
}