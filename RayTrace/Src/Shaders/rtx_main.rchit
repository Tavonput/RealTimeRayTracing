#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "structures.glsl"

// Payload in
layout (location = 0) rayPayloadInEXT hitPayload payload;
hitAttributeEXT vec3 attribs;

// Payload out
layout (location = 1) rayPayloadEXT shadowPayload payloadShadow;

// Acceleration structure
layout (set = 0, binding = 0) uniform accelerationStructureEXT topLevelAS;

// Global uniform buffer
layout (set = 1, binding = 0) uniform _GlobalUniform { GlobalUniform uni; };

// Object buffers
layout (buffer_reference, scalar) buffer VertexBuffer { Vertex v[]; };
layout (buffer_reference, scalar) buffer IndexBuffer { ivec3 i[]; };
layout (buffer_reference, scalar) buffer MaterialBuffer { Material m[]; };
layout (buffer_reference, scalar) buffer MatIndexBuffer { int i[]; };

// Addresses to the object buffers
layout (set = 1, binding = 1) buffer _ObjectDescription { ObjectDescription i[]; } objDesc;

// Texture samplers
layout (set = 1, binding = 2) uniform sampler2D[] textureSamplers;

#include "pbr.glsl"
#include "shade_state.glsl"

vec3 computeDiffuse(Material mat, vec3 normal, vec3 lightDirection)
{
	// Ambient
	vec3 ambient = uni.lightColor * mat.ambient;

	// Diffuse
	float dotNL    = max(dot(normal, lightDirection), 0.0);
	vec3  diffuse  = dotNL * mat.diffuse * uni.lightColor;

	return (ambient + diffuse);
};

vec3 computeSpecular(Material mat, vec3 normal, vec3 viewDirection, vec3 lightDirection)
{
    vec3  halfway  = normalize(lightDirection + viewDirection);  
    float dotNH    = pow(max(dot(normal, halfway), 0.0), mat.shininess);

    return (dotNH * mat.specular * uni.lightColor);  
};

void main()
{
	// Get object buffers
	ObjectDescription objAddresses   = objDesc.i[gl_InstanceCustomIndexEXT];
	MatIndexBuffer    matIndexBuffer = MatIndexBuffer(objAddresses.materialIndexAddress);
	MaterialBuffer    materialBuffer = MaterialBuffer(objAddresses.materialAddress);
	VertexBuffer      vertexBuffer   = VertexBuffer(objAddresses.vertexAddress);
	IndexBuffer       indexBuffer    = IndexBuffer(objAddresses.indexAddress);

	// Indices of the triangle
	ivec3 indices = indexBuffer.i[gl_PrimitiveID];

	// Vertices of the triangle
	Vertex v0 = vertexBuffer.v[indices.x];
	Vertex v1 = vertexBuffer.v[indices.y];
	Vertex v2 = vertexBuffer.v[indices.z];

	// Material
	int      matIndex = matIndexBuffer.i[gl_PrimitiveID];
	Material material = materialBuffer.m[matIndex];

	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

	// Computing world position
	vec3 pos      = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
	vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));

	// Computing world normal
	vec3 normal      = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
	vec3 worldNormal = normalize(vec3(normal * gl_WorldToObjectEXT));

	// Computing world tangent
	vec3 tangent      = v0.tangent * barycentrics.x + v1.tangent * barycentrics.y + v2.tangent * barycentrics.z;
	vec3 worldTangent = normalize(vec3(tangent * gl_WorldToObjectEXT));

	// Computing world bitangent
	vec3 worldBitangent = cross(worldNormal, worldTangent);

	// Compute TBN
	mat3 TBN = mat3(worldTangent, worldBitangent, worldNormal);

	// Computing the texture coordinates at the hit position
	vec2 texCoords = v0.texCoord * barycentrics.x + v1.texCoord * barycentrics.y + v2.texCoord * barycentrics.z;

	vec4  albedo    = vec4(material.diffuse, 1.0);
	float metallic  = material.metallic;
	float roughness = material.roughness;

	if (material.textureID >= 0)
	{
		int txtOffset = objDesc.i[gl_InstanceCustomIndexEXT].txtOffset;
		sampleTextures(material, txtOffset, texCoords, albedo, worldNormal, TBN, metallic, roughness);
	}

	// Lighting
	vec3  N = worldNormal;
	vec3  V = -gl_WorldRayDirectionEXT;
	vec3  L = normalize(uni.lightPosition - worldPos);
	vec3  H = normalize(V + L);

	// Correct normal
	N = faceForwardNormal(N, V);

	float distance    = length(uni.lightPosition - worldPos);
	float attenuation = uni.lightIntensity / (distance * distance);
	vec3  radiance    = uni.lightColor * attenuation;

	vec3 Lo      = cookTorrance(N, V, L, H, albedo, roughness, metallic, radiance);
	vec3 ambient = albedo.rgb * vec3(0.01);
	vec3 color   = Lo + ambient;

	// Initialize shadow payload before tracing
	float shadowComponent = 1.0;
	payloadShadow.isHit   = true;
	payloadShadow.seed    = payload.seed;

	// Trace shadow ray if we are visible
	if (dot(N, L) > 0)
	{
		float tMin   = 0.001;
		float tMax   = distance;
		vec3  origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
		vec3  rayDir = L;
		uint  flags  = gl_RayFlagsSkipClosestHitShaderEXT;
		traceRayEXT(
			topLevelAS,  // acceleration structure
			flags,       // rayFlags
			0xFF,        // cullMask
			1,           // sbtRecordOffset
			0,           // sbtRecordStride
			1,           // missIndex
			origin,      // ray origin
			tMin,        // ray min range
			rayDir,      // ray direction
			tMax,        // ray max range
			1            // payload (location = 1)
		);
	}

	payload.seed = payloadShadow.seed;

	if (payloadShadow.isHit)
	{
		shadowComponent = 0.3;
	}

	// Reflection
	if (material.illum == 3)
	{
		color = vec3(0);
		payload.attenuation *= material.specular;
		payload.done         = 0;
		payload.rayOrigin    = worldPos;
		payload.rayDir       = reflect(gl_WorldRayDirectionEXT, N);
	}

	// Set final color
	switch (uni.debugMode)
	{
		case DEBUG_NONE:
			payload.hitValue = shadowComponent * color;
			break;

		case DEBUG_ALBEDO:
			payload.hitValue = albedo.rgb;
			break;

		case DEBUG_NORMAL:
			payload.hitValue = N;
			break;

		case DEBUG_METAL:
			payload.hitValue = vec3(metallic);
			break;

		case DEBUG_ROUGH:
			payload.hitValue = vec3(roughness);
			break;

		default:
			payload.hitValue = shadowComponent * color;
	}
}
