#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_ray_query : enable

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform accelerationStructureEXT topLevelAS;

void main() {
  vec3 rayOrigin = vec3(0, 0, 0);
  vec3 rayDirection = vec3(0, 0, 1);

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  if (rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT) {
    outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  }
  else {
    outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  }
}
