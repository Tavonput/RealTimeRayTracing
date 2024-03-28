#version 460

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "structures.glsl"
#include "pbr.glsl"
#include "random.glsl"

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

	vec3 albedo = material.diffuse;
	vec3 normal = fragNormal;
	if (material.textureID >= 0)
	{
		// Note: Textures must be sourced in the same order they were added during loading

		int txtID = objDesc.i[pc.objectID].txtOffset + material.textureID;

		// Albedo
		vec4 txt = texture(textureSamplers[nonuniformEXT(txtID)], texCoords);
		if (txt.a < 0.1)
			discard;
		albedo = txt.xyz;

		// Normal
		if ((material.textureMask & NORMAL_BIT) == NORMAL_BIT)
		{
			txtID++;
			normal = texture(textureSamplers[nonuniformEXT(txtID)], texCoords).xyz;
			normal = normal * 2.0 - 1.0;
			normal = TBN * normal;
		}

		// Alpha
		if ((material.textureMask & ALPHA_BIT) == ALPHA_BIT)
		{
			txtID++;
			float alpha = texture(textureSamplers[nonuniformEXT(txtID)], texCoords).a;
			if (alpha < 0.1)
				discard;
		}
	}

	float roughness = material.roughness;
	float metallic  = material.metallic;

	// Lighting
	vec3 N = normalize(normal);
	vec3 V = normalize(uni.viewPosition - fragPos);
	vec3 L = normalize(uni.lightPosition - fragPos);
	vec3 H = normalize(V + L);

	vec3 F0 = vec3(0.04);
	F0      = mix(F0, material.diffuse, metallic);

	float distance    = length(uni.lightPosition - fragPos);
	float attenuation = uni.lightIntensity / (distance * distance);
	vec3  radiance    = uni.lightColor * attenuation;

	float NDF = distributionGGX(N, H, roughness);
	float G   = geometrySmith(N, V, L, roughness);
	vec3  F   = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD     *= 1.0 - metallic;

	vec3  num      = NDF * G * F;
	float den      = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3  specular = num / den;

	const float NdotL = max(dot(N, L), 0.0);

	vec3 Lo = (kD * albedo / PI + specular) * NdotL * radiance;

	vec3 ambient = material.ambient * vec3(0.01);
	vec3 color = Lo + ambient;

	// Set final color
	switch (uni.debugMode)
	{
		case DEBUG_NONE:
			outColor = vec4(color, 1.0);
			break;

		case DEBUG_ALBEDO:
			outColor = vec4(albedo, 1.0);
			break;

		case DEBUG_NORMAL:
			outColor = vec4(N, 1.0);
			break;
	}
}
