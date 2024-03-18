#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#include "structures.glsl"
#include "pbr.glsl"

// Payload in
layout (location = 0) rayPayloadInEXT hitPayload payload;
hitAttributeEXT vec3 attribs;

// Payload out
layout (location = 1) rayPayloadEXT bool isShadowed;

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

	// Computing the coordinates of the hit position
	const vec3 pos      = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
	const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));

	// Computing the normal at hit position
	const vec3 normal      = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
	const vec3 worldNormal = normalize(vec3(normal * gl_WorldToObjectEXT));

	// Lighting properties
	vec3  viewDir        = -gl_WorldRayDirectionEXT;
	vec3  lightDirection = uni.lightPosition - worldPos;
	float lightDistance  = length(lightDirection);
	float lightIntensity = uni.lightIntensity / (lightDistance * lightDistance);
	lightDirection       = normalize(lightDirection);

	// Diffuse
	// vec3 diffuse = computeDiffuse(material, worldNormal, lightDirection);

	const vec3  halfway = normalize(viewDir + lightDirection);
	const float NdotL   = max(dot(worldNormal, lightDirection), 0.0);

	float distance    = length(uni.lightPosition - worldPos);
	float attenuation = uni.lightIntensity / (distance * distance);
	vec3  radiance    = uni.lightColor * attenuation;

	vec3 F0 = vec3(0.04);
	F0      = mix(F0, material.diffuse, material.metallic);

	float NDF = distributionGGX(worldNormal, halfway, material.roughness);
	float G   = geometrySmith(worldNormal, viewDir, lightDirection, material.roughness);
	vec3  F   = fresnelSchlick(clamp(dot(halfway, viewDir), 0.0, 1.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.metallic;

	vec3  num      = NDF * G * F;
	float den      = 4.0 * max(dot(worldNormal, viewDir), 0.0) * NdotL + 0.0001;
	vec3  specular = num / den;

	// Initialize shadow and specular components before tracing
	float shadowComponent = 1;
	// vec3  specular = vec3(0);

	// Trace shadow ray if we are visible
	if (dot(worldNormal, lightDirection) > 0)
	{
		float tMin   = 0.001;
		float tMax   = lightDistance;
		vec3  origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
		vec3  rayDir = lightDirection;
		uint  flags  = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
		isShadowed   = true;
		traceRayEXT(
			topLevelAS,  // acceleration structure
			flags,       // rayFlags
			0xFF,        // cullMask
			0,           // sbtRecordOffset
			0,           // sbtRecordStride
			1,           // missIndex
			origin,      // ray origin
			tMin,        // ray min range
			rayDir,      // ray direction
			tMax,        // ray max range
			1            // payload (location = 1)
		);
	}
	if (isShadowed)
	{
		specular = vec3(0);
		shadowComponent = 0.3;
	}

	vec3 Lo = (kD * material.diffuse / PI + specular) * NdotL * radiance;

	vec3 ambient = material.ambient * vec3(0.01);
	vec3 color   = Lo + ambient;

	// Reflection
	if (material.illum >= 3)
	{
		color = vec3(0);
		payload.attenuation *= material.specular;
		payload.done         = 0;
		payload.rayOrigin    = worldPos;
		payload.rayDir       = reflect(gl_WorldRayDirectionEXT, worldNormal);
	}

	// payload.hitValue = lightIntensity * shadowComponent * (diffuse + specular);
	payload.hitValue = shadowComponent * color;
}
