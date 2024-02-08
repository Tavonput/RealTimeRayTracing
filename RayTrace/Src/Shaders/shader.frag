#version 460

// Main geometry pass

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform constants
{
	mat4 renderMatrix;
	mat4 model;
} pushConstants;

void main()
{
	// Light info
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 lightPosition = vec3(1.2, 1.0, 2.0);

	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = lightColor * ambientStrength;

	// Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPosition - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lightColor * diff;

	vec3 result = (ambient + diffuse) * fragColor;
	outColor = vec4(result, 1.0);
}