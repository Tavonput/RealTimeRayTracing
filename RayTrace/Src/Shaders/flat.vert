#version 460

#extension GL_GOOGLE_include_directive : enable
#extension GL_ARB_gpu_shader_int64 : require

#include "structures.glsl"

// Inputs
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord;

// Global uniform
layout (binding = 0) uniform _GlobalUniform { GlobalUniform uni; };

// Push constant
layout (push_constant) uniform Constants { PushConstant pc; };

void main()
{
	gl_Position = uni.viewProjection * pc.model * vec4(inPosition, 1.0);
}
