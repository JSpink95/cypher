#version 440 core

layout(location = 0) in vec3 aPosition;

layout(std140, binding=0) uniform PerFrame
{
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
};

layout(std140, binding=1) uniform PerObject
{
	mat4 transform;
};

void main()
{
	gl_Position = viewProjection * transform * vec4(aPosition, 1.0);
}
