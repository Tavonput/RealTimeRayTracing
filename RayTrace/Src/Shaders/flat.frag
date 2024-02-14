#version 460

layout(location = 0) out vec4 outColor;

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
	vec3 objectColor;
} pc;

void main()
{
	outColor = vec4(pc.objectColor, 1.0);
}
