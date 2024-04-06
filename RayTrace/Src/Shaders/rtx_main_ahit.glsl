#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "structures.glsl"
#include "random.glsl"

// Payload in
#ifdef PAYLOAD_0
	layout(location = 0) rayPayloadInEXT hitPayload payload;
#elif defined(PAYLOAD_1)
	layout(location = 1) rayPayloadInEXT shadowPayload payload;
#endif
hitAttributeEXT vec3 attribs;

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

	// Computing the texture coordinates at the hit position
	vec2 texCoords = v0.texCoord * barycentrics.x + v1.texCoord * barycentrics.y + v2.texCoord * barycentrics.z;

	vec4 albedo = vec4(material.diffuse, 1.0);
	if (material.textureID >= 0)
	{
		int txtOffset = objDesc.i[gl_InstanceCustomIndexEXT].txtOffset;

		vec3  dummyNormal;
		float dummyMetal, dummyRough;
		mat3  dummyTBN;
		sampleTextures(material, txtOffset, texCoords, albedo, dummyNormal, dummyTBN, dummyMetal, dummyRough);
	}

	if (albedo.a < 0.1)
		ignoreIntersectionEXT;

	if (material.illum != 7)
		return;

	if (material.transmittance.x == 0.0)
		ignoreIntersectionEXT;
	else if (rnd(payload.seed) > material.transmittance.x)
		ignoreIntersectionEXT;
}
