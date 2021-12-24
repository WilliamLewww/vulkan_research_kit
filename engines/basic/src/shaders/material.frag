#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform Camera {
  mat4 viewMatrix;
  mat4 projectionMatrix;
}
camera;

layout(set = 0, binding = 1) uniform Scene { uint lightCount; }
scene;

layout(set = 0, binding = 2) uniform Light {
  vec4 position;
  vec4 direction;
  int type;
}
lights[16];

void main() {
  vec3 color = vec3(1, 1, 1);

  for (uint x = 0; x < scene.lightCount; x++) {
    if (lights[x].type == 1) {
      vec4 surfaceToLight =
          normalize(lights[x].position - vec4(inPosition, 1.0));
      color = color * dot(vec4(inNormal, 0.0), surfaceToLight);
    }
  }

  outColor = vec4(color, 1.0);
}
