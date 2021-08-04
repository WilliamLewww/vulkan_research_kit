#include "vrk/semaphore.h"

Semaphore::Semaphore(VkDevice* deviceHandlePtr) : Component("Semaphore") {
  this->semaphoreHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;
}

Semaphore::~Semaphore() {
  vkDestroySemaphore(*this->deviceHandlePtr, this->semaphoreHandle, NULL);
}

bool Semaphore::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkSemaphoreCreateInfo semaphoreCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0
  };

  VkResult result = vkCreateSemaphore(*this->deviceHandlePtr,
      &semaphoreCreateInfo, NULL, &this->semaphoreHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateSemaphore");
  }

  return true;
}

VkSemaphore* Semaphore::getSemaphoreHandlePtr() {
  return &this->semaphoreHandle;
}