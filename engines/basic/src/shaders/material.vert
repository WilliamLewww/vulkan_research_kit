#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;

layout(set = 0, binding = 0) uniform Camera {
  mat4 viewMatrix;
  mat4 projectionMatrix;
}
camera;

void main() {
  gl_Position = camera.projectionMatrix * inverse(camera.viewMatrix) *
                vec4(inPosition, 1.0);
}
