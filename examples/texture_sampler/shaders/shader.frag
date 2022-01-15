#version 460

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform Colors {
  float red;
  float green;
  float blue;
}
colors;

void main() { outColor = vec4(colors.red, colors.green, colors.blue, 1.0); }