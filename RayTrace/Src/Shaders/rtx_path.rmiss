#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_ARB_gpu_shader_int64 : require

#include "structures.glsl"

layout (location = 0) rayPayloadInEXT hitPayloadPath payload;

layout (push_constant) uniform _RtxPushConstant { RtxPushConstant pc; };

void main()
{
    const float rayDir = (payload.rayDir.y + 1.0) / 2.0;
    const vec3  color  = mix(vec3(0), pc.clearColor.xyz, rayDir);

    payload.emission    = vec3(1.0);
    payload.throughput *= color;

    payload.done = 1;
}
