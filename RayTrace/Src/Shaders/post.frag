#version 460

#extension GL_GOOGLE_include_directive : enable

#extension GL_ARB_gpu_shader_int64 : require

#include "structures.glsl"

layout (location = 0) in vec2 vsUV;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D txt;

layout (push_constant) uniform _PostPushConstant { PostPushConstant pc; };

void main()
{
	vec3 color = texture(txt, vsUV).rgb;

	// Exposure tone map
	color = vec3(1.0) - exp(-color * pc.exposure);

	// Gamma correction
	const float gamma = 2.2;
	color = pow(color, vec3(1.0 / gamma));

	outColor = vec4(color, 1.0);
}
