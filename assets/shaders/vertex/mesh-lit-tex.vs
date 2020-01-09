#version 440 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;

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

out vec3 fPos;
out vec3 fNor;
out vec2 fTex;

void main()
{
	gl_Position = viewProjection * transform * vec4(aPosition, 1.0);
	fPos = (transform * vec4(aPosition, 1.0)).xyz;
	fNor = normalize(mat3(transform) * aNormal);
	fTex = aTexcoord;
}
