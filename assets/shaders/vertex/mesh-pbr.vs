#version 440 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTexcoord;

layout(std140, binding=0) uniform PerFrame
{
	mat4 view, projection;
	mat4 viewProjection;
};

layout(std140, binding=1) uniform PerObject
{
	mat4 transform;
};

out vec3 fWorldPos;
out vec3 fObjectNormal;
out vec2 fTexcoord;

void main()
{
	gl_Position = viewProjection * transform * vec4(aPosition, 1.0);
	fWorldPos = (transform * vec4(aPosition, 1.0)).xyz;
	fObjectNormal = normalize(mat3(transform) * aNormal);
	fTexcoord = aTexcoord;
}