#version 440 core

layout(points) in;
layout(max_vertices=4, triangle_strip) out;

in vec3 gsPosition[];
in vec3 gsColor[];
in vec2 gsSize[];
in vec2 gsUvScale[];
in vec2 gsUvOffset[];

layout(std140, binding=0) uniform PerFrame
{
	mat4 view, projection;
	mat4 viewProjection;
};

layout(std140, binding=1) uniform PerObject
{
	mat4 transform;
};

layout(std140, binding=2) uniform ParticleExtra
{
	vec4 cameraPosition;
};

out vec2 fTexcoord;
out vec4 fColor;

void main()
{
	vec3 particleCentreLocalSpace = gsPosition[0];
	vec2 particleSize = gsSize[0];
	vec2 uvScale = gsUvScale[0];
	vec2 uvOffset = gsUvOffset[0];

	mat4 viewTransform = view * transform;

	vec3 particleCentreWorldSpace = (transform * vec4(particleCentreLocalSpace, 1.0)).xyz;
	vec3 cameraFacingNormal = normalize(cameraPosition.xyz - particleCentreWorldSpace);

	vec3 left = normalize(cross(vec3(0.0, 1.0, 0.0), cameraFacingNormal));
	vec3 up = normalize(cross(cameraFacingNormal, left));

	left *= vec3(particleSize.xy, particleSize.x);
	up *= vec3(particleSize.xy, particleSize.x);

	vec4 a = vec4(particleCentreWorldSpace - left + up, 1.0);	// top left
	vec4 b = vec4(particleCentreWorldSpace - left - up, 1.0); // bottom left
	vec4 c = vec4(particleCentreWorldSpace + left + up, 1.0); // top right
	vec4 d = vec4(particleCentreWorldSpace + left - up, 1.0); // bottom right

	fColor = vec4(gsColor[0].rgb, 1.0);

	gl_Position = viewProjection * b;
	fTexcoord = (vec2(0.0, 0.0) + uvOffset) * uvScale;
	EmitVertex();

	gl_Position = viewProjection * d;
	fTexcoord = (vec2(1.0, 0.0) + uvOffset) * uvScale;
	EmitVertex();

	gl_Position = viewProjection * a;
	fTexcoord = (vec2(0.0, 1.0) + uvOffset) * uvScale;
	EmitVertex();

	gl_Position = viewProjection * c;
	fTexcoord = (vec2(1.0, 1.0) + uvOffset) * uvScale;
	EmitVertex();
}
