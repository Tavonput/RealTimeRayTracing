#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "structures.glsl"
#include "random.glsl"

// Payload in
layout (location = 0) rayPayloadInEXT hitPayloadPath payload;
hitAttributeEXT vec3 attribs;

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

// Push constant
layout (push_constant) uniform _RtxPushConstant { RtxPushConstant pc; };

#include "shade_state.glsl"

void lambertian(vec3 albedo, vec3 N)
{
	vec3 t, b;
	createCoordinateSystem(N, t, b);
	vec3 rayDirection = samplingHemisphere(payload.seed, t, b, N);

	const float cosTheta = dot(rayDirection, N);
	const float p        = cosTheta / PI;

	vec3 BRDF = albedo / PI;

	payload.rayDir      = rayDirection;
	payload.throughput *= BRDF * cosTheta / p;
}

void mirror(vec3 normal)
{
	payload.rayDir = reflect(gl_WorldRayDirectionEXT, normal);
}

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

	// Computing the coordinates of the hit position
	const vec3 pos      = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
	const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));

	// Computing the normal at hit position
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

	// Find the albedo to use
	vec4  albedo    = vec4(material.diffuse, 1.0);
	// vec3  N         = normalize(TBN * worldNormal); normal mapping is not working
	vec3  N         = normalize(worldNormal);
	float metallic  = material.metallic;
	float roughness = material.roughness;

	// Sample textures
	if (material.textureID >= 0)
	{
		int txtOffset = objDesc.i[gl_InstanceCustomIndexEXT].txtOffset;
		vec3 dummyNormal;
		sampleTextures(material, txtOffset, texCoords, albedo, dummyNormal, metallic, roughness);
	}

	if (material.illum == 2 || material.illum == 4)
		lambertian(albedo.xyz, N);
	else if (material.illum == 3)
		mirror(N);

	payload.rayOrigin = worldPos;
	payload.emission  = material.emission * uni.lightIntensity * uni.lightColor;
	payload.done      = 0;
}
