#include "vrk/semaphore.h"

Semaphore::Semaphore(VkDevice* deviceHandlePtr) {
  if (*deviceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid device handle");
  }

  this->isActive = false;

  this->semaphoreHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;
}

Semaphore::~Semaphore() {
  vkDestroySemaphore(*this->deviceHandlePtr, this->semaphoreHandle, NULL);
}

void Semaphore::activate() {
  if (this->isActive) {
    PRINT_MESSAGE(std::cerr, "Semaphore is already active");
    return;
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

  this->isActive = true;
}

VkSemaphore* Semaphore::getSemaphoreHandlePtr() {
  return &this->semaphoreHandle;
}

std::ostream& operator<<(std::ostream& os, const Semaphore& semaphore) {
  os << "semaphore: " << &semaphore << std::endl;
  os << "  semaphore handle: " << semaphore.semaphoreHandle << std::endl;
  os << "  device handle (ptr): " << *semaphore.deviceHandlePtr;

  return os;
}