#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) in int inMaterialPropertiesIndex;
layout(location = 4) in int inModelIndex;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTextureCoordinate;
layout(location = 3) flat out int outMaterialPropertiesIndex;

layout(set = 0, binding = 0) uniform Camera {
  mat4 viewMatrix;
  mat4 projectionMatrix;
}
camera;

layout(set = 0, binding = 6) uniform Model { mat4 modelMatrix; }
models[32];

void main() {
  gl_Position = camera.projectionMatrix * inverse(camera.viewMatrix) *
                models[inModelIndex].modelMatrix * vec4(inPosition, 1.0);

  outPosition = inPosition;
  outNormal = inNormal;
  outTextureCoordinate = inTextureCoordinate;
  outMaterialPropertiesIndex = inMaterialPropertiesIndex;
}
