#version 460

// Main geometry pass

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 fragPos;

layout(push_constant) uniform constants
{
	mat4 renderMatrix;
	mat4 model;
} pushConstants;

void main()
{
	gl_Position = pushConstants.renderMatrix * vec4(inPosition, 1.0);
	fragColor = inColor;
	normal = mat3(transpose(inverse(pushConstants.model))) * inNormal;
	fragPos = vec3(pushConstants.model * vec4(inPosition, 1.0));
}
