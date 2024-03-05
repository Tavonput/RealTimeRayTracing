#version 460

#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "structures.glsl"

// Inputs
layout (location = 0) in vec3 fragNormal;
layout (location = 1) in vec3 fragPos;
layout (location = 2) in vec2 texCoords;

// Outputs
layout (location = 0) out vec4 outColor;

// Global uniform buffer
layout (binding = 0) uniform _GlobalUniform { GlobalUniform uni; };

// Material storage buffers
layout (buffer_reference) buffer MaterialBuffer { Material m[]; };
layout (buffer_reference) buffer MatIndexBuffer { int i[]; };

// Addresses to the material storage buffers
layout (binding = 1) buffer MaterialDescription_ { MaterialDescription i[]; } matDesc;

// Push constant
layout (push_constant) uniform Constants { PushConstant pc; };

vec3 computeLighting(Material mat, vec3 normal, vec3 viewDirection, vec3 lightDirection)
{
	// Ambient
	vec3 ambient = uni.lightColor * mat.ambient;

	// Diffuse
	float dotNL    = max(dot(normal, lightDirection), 0.0);
	vec3  diffuse  = dotNL * mat.diffuse * uni.lightColor;

	// Specular
    vec3  halfway  = normalize(lightDirection + viewDirection);  
    float dotNH    = pow(max(dot(normal, halfway), 0.0), mat.shininess);
    vec3  specular = dotNH * mat.specular * uni.lightColor;  

	// Attenuation
	float distance = length(uni.lightPosition - fragPos);
	float attenuation = uni.lightIntensity / (distance * distance);

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main()
{
	// Get material buffers
	MaterialDescription matAddresses   = matDesc.i[pc.objectID];
	MatIndexBuffer      matIndexBuffer = MatIndexBuffer(matAddresses.materialIndexAddress);
	MaterialBuffer      materialBuffer = MaterialBuffer(matAddresses.materialAddress);

	// Get the material
	int      matIndex = matIndexBuffer.i[gl_PrimitiveID];
	Material material = materialBuffer.m[matIndex];

	// Lighting
	vec3 norm     = normalize(fragNormal);
	vec3 viewDir  = normalize(uni.viewPosition - fragPos);
	vec3 lightDir = normalize(uni.lightPosition - fragPos);

	vec3 color     = vec3(0.0);
	color += computeLighting(material, norm, viewDir, lightDir);

	outColor = vec4(color, 1.0);
}