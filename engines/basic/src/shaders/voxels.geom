#version 460

layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;

flat layout(location = 0) out vec4 outVoxelCenter;

layout(set = 0, binding = 0) uniform Camera {
  mat4 viewMatrix;
  mat4 projectionMatrix;
}
camera;

vec4 applyCameraMatrices(vec4 position) {
  return camera.projectionMatrix * inverse(camera.viewMatrix) * position;
}

void createFace(float voxelSize, vec4 voxelCenter, vec4 direction) {
  if (direction.x == -1) {
    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, -voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, -voxelSize, voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, voxelSize, voxelSize, 0.0));
    EmitVertex();
  } else if (direction.x == 1) {
    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, -voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, -voxelSize, voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, voxelSize, voxelSize, 0.0));
    EmitVertex();
  } else if (direction.y == -1) {
    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, -voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, -voxelSize, voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, -voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, -voxelSize, voxelSize, 0.0));
    EmitVertex();
  } else if (direction.y == 1) {
    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, voxelSize, voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, voxelSize, voxelSize, 0.0));
    EmitVertex();
  } else if (direction.z == -1) {
    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, -voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, -voxelSize, -voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, voxelSize, -voxelSize, 0.0));
    EmitVertex();
  } else if (direction.z == 1) {
    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, -voxelSize, voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(-voxelSize, voxelSize, voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, -voxelSize, voxelSize, 0.0));
    EmitVertex();

    gl_Position = applyCameraMatrices(
        voxelCenter + vec4(voxelSize, voxelSize, voxelSize, 0.0));
    EmitVertex();
  }

  outVoxelCenter = voxelCenter;
  EndPrimitive();
}

void main() {
  float voxelSize = 0.1;
  vec4 voxelCenter = ceil(gl_in[0].gl_Position / voxelSize) * voxelSize;

  createFace(voxelSize, voxelCenter, vec4(-1, 0, 0, 0));
  createFace(voxelSize, voxelCenter, vec4(1, 0, 0, 0));
  createFace(voxelSize, voxelCenter, vec4(0, -1, 0, 0));
  createFace(voxelSize, voxelCenter, vec4(0, 1, 0, 0));
  createFace(voxelSize, voxelCenter, vec4(0, 0, -1, 0));
  createFace(voxelSize, voxelCenter, vec4(0, 0, 1, 0));
}
