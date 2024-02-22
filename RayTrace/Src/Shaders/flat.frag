#version 460

#extension GL_ARB_gpu_shader_int64 : require

#include "structures.glsl"

// Output
layout (location = 0) out vec4 outColor;

// Global uniform
layout (binding = 0) uniform _GlobalUniform { GlobalUniform uni; };

// Push constant
layout (push_constant) uniform Constants { PushConstant pc; };

void main()
{
	outColor = vec4(pc.objectColor, 1.0);
}
