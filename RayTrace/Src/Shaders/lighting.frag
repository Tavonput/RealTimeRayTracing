#version 460

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 fragPos;

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
} pc;

void main()
{
	// Ambient
	vec3 ambient = 0.1 * fragColor;

	// Diffuse
	vec3 norm     = normalize(normal);
	vec3 lightDir = normalize(ubo.lightPosition - fragPos);
	float diff    = max(dot(norm, lightDir), 0.0);
	vec3 diffuse  = diff * fragColor;

	// Specular
	vec3 viewDir    = normalize(ubo.viewPosition - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    vec3 specular   = vec3(0.3) * spec;  

	// Attenuation
	float constant  = 1.0;
	float linear    = 0.35;
	float quadratic = 0.44;

	float distance    = length(ubo.lightPosition - fragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	outColor = vec4(ambient + diffuse + specular, 1.0);
}