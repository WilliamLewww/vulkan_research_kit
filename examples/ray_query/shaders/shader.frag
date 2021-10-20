#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_ray_query : enable

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform accelerationStructureEXT topLevelAS;

void main() {
  vec3 rayOrigin = vec3(-((gl_FragCoord.x / 800.0) - 0.5),
                        ((gl_FragCoord.y / 600.0) - 0.5), 0.0);
  vec3 rayDirection = vec3(0, 0, 1);

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF,
                        rayOrigin, 0.0001f, rayDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery))
    ;

  vec2 barycentric = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);

  outColor = vec4(barycentric.x, barycentric.y,
                  1.0 - barycentric.x - barycentric.y, 1.0f);
}
