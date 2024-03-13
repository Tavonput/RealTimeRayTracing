#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#include "structures.glsl"

// Payload
layout (location = 0) rayPayloadInEXT hitPayload payload;
hitAttributeEXT vec3 attribs;

// Global uniform buffer
layout (set = 1, binding = 0) uniform _GlobalUniform { GlobalUniform uni; };

// Object buffers
layout (buffer_reference, scalar) buffer VertexBuffer { Vertex v[]; };
layout (buffer_reference, scalar) buffer IndexBuffer { ivec3 i[]; };
layout (buffer_reference, scalar) buffer MaterialBuffer { Material m[]; };
layout (buffer_reference, scalar) buffer MatIndexBuffer { int i[]; };

// Addresses to the material storage buffers
layout (set = 1, binding = 1) buffer _ObjectDescription { ObjectDescription i[]; } objDesc;

vec3 computeLighting(Material mat, vec3 normal, vec3 viewDirection, vec3 lightDirection, float lightIntensity)
{
	// Ambient
	vec3 ambient = uni.lightColor * mat.ambient * lightIntensity;

	// Diffuse
	float dotNL    = max(dot(normal, lightDirection), 0.0);
	vec3  diffuse  = dotNL * mat.diffuse * uni.lightColor * lightIntensity;

	// Specular
    vec3  halfway  = normalize(lightDirection + viewDirection);  
    float dotNH    = pow(max(dot(normal, halfway), 0.0), mat.shininess);
    vec3  specular = dotNH * mat.specular * uni.lightColor * lightIntensity;  

	return (ambient + diffuse + specular);
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
	const vec3 normal      = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
	const vec3 worldNormal = normalize(vec3(normal * gl_WorldToObjectEXT));

	// Lighting
	vec3  lightDirection = uni.lightPosition - worldPos;
	float lightDistance  = length(lightDirection);
	float lightIntensity = uni.lightIntensity / (lightDistance * lightDistance);
	lightDirection       = normalize(lightDirection);

	vec3 color = vec3(0.0);
	color += computeLighting(material, worldNormal, gl_WorldRayDirectionEXT, lightDirection, lightIntensity);

	payload.hitValue = color;
}
