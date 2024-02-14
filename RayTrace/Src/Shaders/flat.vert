#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(binding = 0) uniform GlobalUnfiform
{
	mat4 viewProjection;

	vec3 lightPosition;
	vec3 lightColor;

	vec3 viewPosition;
} ubo;

layout(push_constant) uniform constants
{
	mat4 model;
} pc;

void main()
{
	gl_Position = ubo.viewProjection * pc.model * vec4(inPosition, 1.0);
}
