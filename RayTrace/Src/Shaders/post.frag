#version 460

layout (location = 0) in vec2 vsUV;

layout (location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D noisyTxt;

void main()
{
  vec2  uv    = vsUV;
  float gamma = 1.0 / 2.2;
  outColor   = pow(texture(noisyTxt, uv).rgba, vec4(gamma));
}
