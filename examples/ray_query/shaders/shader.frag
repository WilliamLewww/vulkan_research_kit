#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_ray_query : enable

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform Colors {
  float red;
  float green;
  float blue;
} colors;

layout(set = 0, binding = 1) uniform accelerationStructureEXT topLevelAS;

void main() {
  vec3 rayOrigin = vec3(0, 0, 0);
  vec3 rayDirection = vec3(0, 0, 1);

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  outColor = vec4(colors.red, colors.green, colors.blue, 1.0f);
}
