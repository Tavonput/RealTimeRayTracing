#version 460

layout (location = 0) out vec2 vsUV;

void main()
{
	vsUV       = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(vsUV * 2.0 - 1.0, 1.0, 1.0);
}
