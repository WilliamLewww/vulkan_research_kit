#pragma once

#include <vrk/buffer.h>

#include <cmath>
#include <cstring>
#include <memory>
#include <string>

class Engine;

class Camera {
public:
  struct CameraShaderStructure {
    alignas(16) float viewMatrix[16];
    alignas(16) float projectionMatrix[16];
  };

  Camera(std::string cameraName, std::shared_ptr<Engine> enginePtr);

  ~Camera();

  void setPosition(float x, float y, float z);

  void updatePosition(float x, float y, float z);

  void setRotation(float yaw, float pitch, float roll);

  void updateRotation(float yaw, float pitch, float roll);

  bool getIsCameraBufferDirty();

  void resetIsCameraBufferDirty();

  std::shared_ptr<VkDescriptorBufferInfo> getCameraDescriptorBufferInfoPtr();

private:
  std::string cameraName;

  std::shared_ptr<Engine> enginePtr;

  std::unique_ptr<Buffer> cameraBufferPtr;

  std::shared_ptr<VkDescriptorBufferInfo> cameraDescriptorBufferInfoPtr;

  bool isCameraBufferDirty;

  CameraShaderStructure cameraShaderStructure;

  float position[3];

  float yaw;

  float pitch;

  float roll;

  float fov;

  float aspect;

  float nearPlane;

  float farPlane;

  void updateViewMatrix();

  void updateProjectionMatrix();
};
