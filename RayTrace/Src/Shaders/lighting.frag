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

vec3 pointLight(vec3 lightPosition, vec3 lightColor, vec3 normal, vec3 fragPosition, vec3 viewDirection)
{
	// Ambient
	vec3 ambient = 0.05 * fragColor;

	// Diffuse
	vec3 lightDir = normalize(lightPosition - fragPosition);
	float diff    = max(dot(normal, lightDir), 0.0);
	vec3 diffuse  = diff * lightColor;

	// Specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec      = pow(max(dot(viewDirection, reflectDir), 0.0), 32.0);
    vec3 specular   = vec3(0.3) * spec * lightColor;  

	// Attenuation
	float constant  = 1.0;
	float linear    = 0.35;
	float quadratic = 0.44;

	float distance    = length(ubo.lightPosition - fragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 directionalLight(vec3 lightDirection, vec3 lightColor, vec3 normal, vec3 viewDirection)
{
	// Ambient
	vec3 ambient = 0.005 * fragColor;

	// Diffuse
	vec3 lightDir = normalize(lightDirection);
	float diff    = max(dot(normal, lightDir), 0.0);
	vec3 diffuse  = diff * lightColor;

	// Specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec      = pow(max(dot(viewDirection, reflectDir), 0.0), 8.0);
    vec3 specular   = vec3(0.03) * spec * lightColor;  

	return (ambient + diffuse + specular);
}

void main()
{
	vec3 sunDirection = vec3(0.5, 0.0, -1.0);
	vec3 sunColor     = vec3(1.0, 1.0, 1.0);
	vec3 norm         = normalize(normal);
	vec3 viewDir      = normalize(ubo.viewPosition - fragPos);
	
	vec3 lighting = vec3(0.0);

	// Directional light
	// lighting += directionalLight(sunDirection, sunColor, norm, viewDir);

	// Point light
	lighting += pointLight(ubo.lightPosition, ubo.lightColor, norm, fragPos, viewDir);

	outColor = vec4(lighting * fragColor, 1.0);
}