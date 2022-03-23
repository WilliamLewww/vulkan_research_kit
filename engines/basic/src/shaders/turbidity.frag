#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) flat in int inMaterialPropertiesIndex;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform Camera {
  mat4 viewMatrix;
  mat4 projectionMatrix;
}
camera;

float inScatter(vec3 start, vec3 dir, vec3 lightPos, float d) {
  vec3 q = start - lightPos;

  float b = dot(dir, q);
  float c = dot(q, q);

  float s = 1.0f / sqrt(c - b * b);
  float l = s * (atan((d + b) * s) - atan(b * s));

  return l;
}

float attenuation(float d, float a, float b) {
  return 1.0 / (1.0 + a * d + b * d * d);
}

vec3 inScatter1(vec3 pmc, vec3 cp, vec3 cd, vec3 lp, float d) {
  vec3 q = cp - lp;

  float b = dot(cd, q);
  float c = dot(q, q);

  float s = 1.0f / sqrt(c - b * b);
  float l = s * (atan((d + b) * s) - atan(b * s));

  return pmc * l * 10.0;
}

vec3 inScatter2(vec3 pmc, float a, float d) { return pmc * (1.0 - exp(a * d)); }

void main() {
  vec3 cameraPosition = vec3(camera.viewMatrix[3]);
  vec3 cameraDirection = normalize(inPosition - cameraPosition);
  float d = distance(cameraPosition, inPosition);
  vec3 lightPosition = vec3(inPosition[0], 10, inPosition[2]);

  vec3 oceanColor = vec3(0.16, 0.29, 0.37);

  vec3 surfaceColor = vec3(1.0, 1.0, 1.0);
  vec3 surfaceToLight = normalize(lightPosition - inPosition);
  vec3 reflectedColor = surfaceColor * dot(-inNormal, surfaceToLight);
  reflectedColor *= attenuation(d, 2.5, 7.5);

  vec3 turbidityColor =
      inScatter1(oceanColor, cameraPosition, cameraDirection, lightPosition, d);

  // vec3 turbidityColor2 = inScatter2(oceanColor, -0.1, d);

  outColor = vec4(reflectedColor + turbidityColor, 1.0);
}
