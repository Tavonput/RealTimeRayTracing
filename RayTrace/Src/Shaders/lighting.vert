#version 460

#extension GL_ARB_gpu_shader_int64 : require

#include "structures.glsl"

// Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

// Output
layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec2 texCoords;

// Global uniform
layout(binding = 0) uniform _GlobalUniform{ GlobalUniform uni; };

// Push Constant
layout(push_constant) uniform Constants { PushConstant pc; };

void main()
{
	vec4 worldPosFull = pc.model * vec4(inPosition, 1.0);

	fragPos     = vec3(worldPosFull);
	gl_Position = uni.viewProjection * worldPosFull;

	texCoords  = inTexCoord;
	fragNormal = mat3(transpose(inverse(pc.model))) * inNormal;	
}
