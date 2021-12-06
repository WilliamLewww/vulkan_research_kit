#version 460

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform Scene { uint lightCount; }
scene;

layout(set = 0, binding = 2) uniform Light {
  vec4 position;
  vec4 direction;
  uint type;
}
lights[16];

void main() { 
  if (lights[0].type == 0) {
    outColor = vec4(0.0, 1.0, 0.0, 1.0);
  }
  else {
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
}
