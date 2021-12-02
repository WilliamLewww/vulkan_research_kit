#include "basic/camera.h"
#include "basic/engine.h"

Camera::Camera(std::string cameraName, std::shared_ptr<Engine> enginePtr)
    : cameraName(cameraName), enginePtr(enginePtr) {

  this->position[0] = 0.0;
  this->position[1] = 0.0;
  this->position[2] = 0.0;

  this->yaw = 0.0;
  this->pitch = 0.0;
  this->roll = 0.0;

  this->fov = 1.0472;
  this->aspect = 800.0 / 600.0;
  this->nearPlane = 0.0001;
  this->farPlane = 1000.0;

  this->cameraBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->devicePtr->getDeviceHandleRef(),
      *enginePtr->physicalDeviceHandlePtr.get(), 0, sizeof(float) * 32,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {enginePtr->queueFamilyIndex}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  this->cameraDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = this->cameraBufferPtr->getBufferHandleRef(),
          .offset = 0,
          .range = VK_WHOLE_SIZE});

  updateViewMatrix();
  updateProjectionMatrix();
}

Camera::~Camera() {}

void Camera::updatePosition(float x, float y, float z) {
  this->position[0] += x;
  this->position[1] += y;
  this->position[2] += z;

  this->updateViewMatrix();
}

void Camera::updateRotation(float yaw, float pitch, float roll) {
  this->yaw += yaw;
  this->pitch += pitch;
  this->roll += roll;

  this->updateViewMatrix();
}

bool Camera::getIsCameraBufferDirty() { return this->isCameraBufferDirty; }

void Camera::resetIsCameraBufferDirty() { this->isCameraBufferDirty = false; }

std::shared_ptr<VkDescriptorBufferInfo>
Camera::getCameraDescriptorBufferInfoPtr() {
  return this->cameraDescriptorBufferInfoPtr;
}

void Camera::updateViewMatrix() {
  viewMatrix[0] = cosf(this->pitch) * cosf(this->yaw);
  viewMatrix[1] = sinf(this->pitch) * cosf(this->yaw);
  viewMatrix[2] = -sinf(this->yaw);
  viewMatrix[3] = 0.0;

  viewMatrix[4] = cosf(this->pitch) * sinf(this->yaw) * sinf(this->roll) -
                  sinf(this->pitch) * cosf(roll);
  viewMatrix[5] = sinf(this->pitch) * sinf(this->yaw) * sinf(this->roll) +
                  cosf(this->pitch) * cosf(roll);
  viewMatrix[6] = cosf(this->yaw) * sinf(this->roll);
  viewMatrix[7] = 0.0;

  viewMatrix[8] = cosf(this->pitch) * sinf(this->yaw) * cosf(this->roll) +
                  sinf(this->pitch) * sinf(roll);
  viewMatrix[9] = sinf(this->pitch) * sinf(this->yaw) * cosf(this->roll) -
                  cosf(this->pitch) * sinf(roll);
  viewMatrix[10] = cosf(this->yaw) * cosf(this->roll);
  viewMatrix[11] = 0.0;

  viewMatrix[12] = this->position[0];
  viewMatrix[13] = this->position[1];
  viewMatrix[14] = this->position[2];
  viewMatrix[15] = 1.0;

  void *hostCameraBuffer;
  this->cameraBufferPtr->mapMemory(&hostCameraBuffer, 0, 32 * sizeof(float));
  memcpy(hostCameraBuffer, this->viewMatrix, 16 * sizeof(float));
  this->cameraBufferPtr->unmapMemory();

  this->isCameraBufferDirty = true;
}

void Camera::updateProjectionMatrix() {
  float frustumDepth = this->farPlane - this->nearPlane;
  float oneOverDepth = 1.0 / frustumDepth;

  this->projectionMatrix[0] = 1.0 / tan(0.5f * this->fov) / this->aspect;
  this->projectionMatrix[1] = 0.0;
  this->projectionMatrix[2] = 0.0;
  this->projectionMatrix[3] = 0.0;

  this->projectionMatrix[4] = 0.0;
  this->projectionMatrix[5] = 1.0 / tan(0.5f * this->fov);
  this->projectionMatrix[6] = 0.0;
  this->projectionMatrix[7] = 0.0;

  this->projectionMatrix[8] = 0.0;
  this->projectionMatrix[9] = 0.0;
  this->projectionMatrix[10] = this->farPlane * oneOverDepth;
  this->projectionMatrix[11] = 1.0;

  this->projectionMatrix[12] = 0.0;
  this->projectionMatrix[13] = 0.0;
  this->projectionMatrix[14] =
      (-this->farPlane * this->nearPlane) * oneOverDepth;
  this->projectionMatrix[15] = 0.0;

  void *hostCameraBuffer;
  this->cameraBufferPtr->mapMemory(&hostCameraBuffer, 0, 32 * sizeof(float));
  memcpy((float *)hostCameraBuffer + 16, this->projectionMatrix,
         16 * sizeof(float));
  this->cameraBufferPtr->unmapMemory();

  this->isCameraBufferDirty = true;
}
