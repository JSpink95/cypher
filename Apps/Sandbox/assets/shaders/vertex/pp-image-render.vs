#version 440 core

layout(location = 0) in vec2 aPosition;

layout(std140, binding=0) uniform ImageDataBuffer
{
	vec2 position;
	vec2 size;
};

out vec2 fTexcoord;

void main()
{
	gl_Position = vec4(position + aPosition * size, 0.0, 1.0);
	fTexcoord = aPosition * 0.5 + 0.5;
}