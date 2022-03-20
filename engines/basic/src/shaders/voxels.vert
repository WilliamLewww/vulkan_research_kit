#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) in int inMaterialPropertiesIndex;
layout(location = 4) in int inModelIndex;

layout(set = 1, binding = 6) uniform Model { mat4 modelMatrix; }
models[32];

void main() {
  gl_Position = models[inModelIndex].modelMatrix * vec4(inPosition, 1.0);
}
