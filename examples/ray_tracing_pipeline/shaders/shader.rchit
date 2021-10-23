#version 460
#extension GL_EXT_ray_tracing : require

hitAttributeEXT vec2 hitCoordinate;

layout(location = 0) rayPayloadInEXT Payload { vec3 color; }
payload;

void main() {
  payload.color = vec3(hitCoordinate.x, hitCoordinate.y,
                       1.0 - hitCoordinate.x - hitCoordinate.y);
}
