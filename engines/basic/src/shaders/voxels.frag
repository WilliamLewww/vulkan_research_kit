#version 460

#define M_PI 3.1415926535897932384626433832795

flat layout(location = 0) in vec4 voxelCenter;

layout(location = 0) out vec4 outColor;

float random(vec2 uv, float seed) {
  return fract(sin(mod(dot(uv, vec2(12.9898, 78.233)) + 1113.1 * seed, M_PI)) *
               43758.5453);
  ;
}

void main() {
  float value = random(voxelCenter.xy, voxelCenter.z);

  outColor = vec4(value);
}
