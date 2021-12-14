#version 460

layout(location = 0) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform Scene { uint lightCount; }
scene;

layout(set = 0, binding = 2) uniform Light {
  vec4 position;
  vec4 direction;
  int type;
}
lights[16];

void main() {
  outColor = vec4(abs(inNormal), 1.0);
}
