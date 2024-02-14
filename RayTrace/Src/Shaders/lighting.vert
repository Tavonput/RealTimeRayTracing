#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 fragPos;

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
	vec4 worldPosFull = pc.model * vec4(inPosition, 1.0);
	fragPos = vec3(worldPosFull);
	gl_Position = ubo.viewProjection * worldPosFull;

	fragColor = inColor;
	normal = mat3(transpose(inverse(pc.model))) * inNormal;	
}
