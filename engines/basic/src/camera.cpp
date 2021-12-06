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
      *enginePtr->physicalDeviceHandlePtr.get(), 0,
      sizeof(CameraShaderStructure), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {enginePtr->queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  this->cameraDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = this->cameraBufferPtr->getBufferHandleRef(),
          .offset = 0,
          .range = VK_WHOLE_SIZE});

  this->updateViewMatrix();
  this->updateProjectionMatrix();
}

Camera::~Camera() {}

void Camera::setPosition(float x, float y, float z) {
  this->position[0] = x;
  this->position[1] = y;
  this->position[2] = z;

  this->updateViewMatrix();
}

void Camera::updatePosition(float x, float y, float z) {
  this->position[0] += x;
  this->position[1] += y;
  this->position[2] += z;

  this->updateViewMatrix();
}

void Camera::setRotation(float yaw, float pitch, float roll) {
  this->yaw = yaw;
  this->pitch = pitch;
  this->roll = roll;

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
  this->cameraShaderStructure.viewMatrix[0] =
      cosf(this->pitch) * cosf(this->yaw);
  this->cameraShaderStructure.viewMatrix[1] =
      sinf(this->pitch) * cosf(this->yaw);
  this->cameraShaderStructure.viewMatrix[2] = -sinf(this->yaw);
  this->cameraShaderStructure.viewMatrix[3] = 0.0;

  this->cameraShaderStructure.viewMatrix[4] =
      cosf(this->pitch) * sinf(this->yaw) * sinf(this->roll) -
      sinf(this->pitch) * cosf(roll);
  this->cameraShaderStructure.viewMatrix[5] =
      sinf(this->pitch) * sinf(this->yaw) * sinf(this->roll) +
      cosf(this->pitch) * cosf(roll);
  this->cameraShaderStructure.viewMatrix[6] =
      cosf(this->yaw) * sinf(this->roll);
  this->cameraShaderStructure.viewMatrix[7] = 0.0;

  this->cameraShaderStructure.viewMatrix[8] =
      cosf(this->pitch) * sinf(this->yaw) * cosf(this->roll) +
      sinf(this->pitch) * sinf(roll);
  this->cameraShaderStructure.viewMatrix[9] =
      sinf(this->pitch) * sinf(this->yaw) * cosf(this->roll) -
      cosf(this->pitch) * sinf(roll);
  this->cameraShaderStructure.viewMatrix[10] =
      cosf(this->yaw) * cosf(this->roll);
  this->cameraShaderStructure.viewMatrix[11] = 0.0;

  this->cameraShaderStructure.viewMatrix[12] = this->position[0];
  this->cameraShaderStructure.viewMatrix[13] = this->position[1];
  this->cameraShaderStructure.viewMatrix[14] = this->position[2];
  this->cameraShaderStructure.viewMatrix[15] = 1.0;

  void *hostCameraBuffer;
  this->cameraBufferPtr->mapMemory(&hostCameraBuffer, 0,
                                   sizeof(CameraShaderStructure));
  memcpy(((CameraShaderStructure *)hostCameraBuffer)->viewMatrix,
         this->cameraShaderStructure.viewMatrix, 16 * sizeof(float));
  this->cameraBufferPtr->unmapMemory();

  this->isCameraBufferDirty = true;
}

void Camera::updateProjectionMatrix() {
  float frustumDepth = this->farPlane - this->nearPlane;
  float oneOverDepth = 1.0 / frustumDepth;

  this->cameraShaderStructure.projectionMatrix[0] =
      1.0 / tan(0.5f * this->fov) / this->aspect;
  this->cameraShaderStructure.projectionMatrix[1] = 0.0;
  this->cameraShaderStructure.projectionMatrix[2] = 0.0;
  this->cameraShaderStructure.projectionMatrix[3] = 0.0;

  this->cameraShaderStructure.projectionMatrix[4] = 0.0;
  this->cameraShaderStructure.projectionMatrix[5] = 1.0 / tan(0.5f * this->fov);
  this->cameraShaderStructure.projectionMatrix[6] = 0.0;
  this->cameraShaderStructure.projectionMatrix[7] = 0.0;

  this->cameraShaderStructure.projectionMatrix[8] = 0.0;
  this->cameraShaderStructure.projectionMatrix[9] = 0.0;
  this->cameraShaderStructure.projectionMatrix[10] =
      this->farPlane * oneOverDepth;
  this->cameraShaderStructure.projectionMatrix[11] = 1.0;

  this->cameraShaderStructure.projectionMatrix[12] = 0.0;
  this->cameraShaderStructure.projectionMatrix[13] = 0.0;
  this->cameraShaderStructure.projectionMatrix[14] =
      (-this->farPlane * this->nearPlane) * oneOverDepth;
  this->cameraShaderStructure.projectionMatrix[15] = 0.0;

  void *hostCameraBuffer;
  this->cameraBufferPtr->mapMemory(&hostCameraBuffer, 0,
                                   sizeof(CameraShaderStructure));
  memcpy(((CameraShaderStructure *)hostCameraBuffer)->projectionMatrix,
         this->cameraShaderStructure.projectionMatrix, 16 * sizeof(float));
  this->cameraBufferPtr->unmapMemory();

  this->isCameraBufferDirty = true;
}
