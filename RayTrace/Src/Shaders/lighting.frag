#version 460

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "structures.glsl"

// Inputs
layout (location = 0) in vec3 fragNormal;
layout (location = 1) in vec3 fragPos;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat3 TBN;

// Outputs
layout (location = 0) out vec4 outColor;

// Global uniform buffer
layout (binding = 0) uniform _GlobalUniform { GlobalUniform uni; };

// Material storage buffers
layout (buffer_reference, scalar) buffer MaterialBuffer { Material m[]; };
layout (buffer_reference, scalar) buffer MatIndexBuffer { int i[]; };

// Addresses to the storage buffers
layout (binding = 1) buffer ObjectDescription_ { ObjectDescription i[]; } objDesc;

// Texture samplers
layout (binding = 2) uniform sampler2D[] textureSamplers;

// Push constant
layout (push_constant) uniform Constants { PushConstant pc; };

#include "pbr.glsl"
#include "random.glsl"
#include "shade_state.glsl"

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
	ObjectDescription objAddresses   = objDesc.i[pc.objectID];
	MatIndexBuffer    matIndexBuffer = MatIndexBuffer(objAddresses.materialIndexAddress);
	MaterialBuffer    materialBuffer = MaterialBuffer(objAddresses.materialAddress);

	// Get the material
	int      matIndex = matIndexBuffer.i[gl_PrimitiveID];
	Material material = materialBuffer.m[matIndex];

	vec4  albedo    = vec4(material.diffuse, 1.0);
	vec3  normal    = fragNormal;
	float metallic  = material.metallic;
	float roughness = material.roughness;

	if (material.textureID >= 0)
	{
		int txtOffset = objDesc.i[pc.objectID].txtOffset;
		vec3 dummyNormal;
		sampleTextures(material, txtOffset, texCoords, albedo, dummyNormal, metallic, roughness);
	}

	// Throw out transparent pixels
	if (albedo.a < 0.1)
		discard;

	// Lighting
	// vec3 N = normalize(TBN * normal); normal mapping is not working
	vec3 N = normalize(normal);
	vec3 V = normalize(uni.viewPosition - fragPos);
	vec3 L = normalize(uni.lightPosition - fragPos);
	vec3 H = normalize(V + L);

	float distance    = length(uni.lightPosition - fragPos);
	float attenuation = uni.lightIntensity / (distance * distance);
	vec3  radiance    = uni.lightColor * attenuation;

	vec3 Lo      = cookTorrance(N, V, L, H, albedo, roughness, metallic, radiance);
	vec3 ambient = albedo.rgb * vec3(0.01);
	vec3 color   = Lo + ambient;

	// Set final color
	switch (uni.debugMode)
	{
		case DEBUG_NONE:
			outColor = vec4(color, 1.0);
			break;

		case DEBUG_ALBEDO:
			outColor = vec4(albedo.rgb, 1.0);
			break;

		case DEBUG_NORMAL:
			outColor = vec4(N, 1.0);
			break;

		case DEBUG_METAL:
			outColor = vec4(vec3(metallic), 1.0);
			break;

		case DEBUG_ROUGH:
			outColor = vec4(vec3(roughness), 1.0);
			break;

		case DEBUG_EXTRA:
			outColor = vec4(N, 1.0);
			break;

		default:
			outColor = vec4(color, 1.0);
	}
}
