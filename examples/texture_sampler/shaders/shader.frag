#version 460

layout(location = 0) in vec2 inTextureCoordinate;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler imageSampler;
layout(set = 0, binding = 1) uniform texture2D textureImage;

void main() {
  outColor =
      texture(sampler2D(textureImage, imageSampler), inTextureCoordinate);
}
