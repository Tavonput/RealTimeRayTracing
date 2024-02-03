#version 460

//Shader file responsible for rendering the triangle. 

layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(fragColor, 1.0);
}