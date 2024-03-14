#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require

#include "structures.glsl"

layout (location = 0) rayPayloadInEXT hitPayload payload;

layout (push_constant) uniform _RtxPushConstant { RtxPushConstant pc; };

void main()
{
    payload.hitValue = pc.clearColor.xyz;
}