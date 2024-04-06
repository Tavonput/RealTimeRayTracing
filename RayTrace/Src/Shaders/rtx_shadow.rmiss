#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require

#include "structures.glsl"

layout(location = 1) rayPayloadInEXT shadowPayload payload;

void main()
{
	payload.isHit = false;
}
