#include "vrk/semaphore.h"

Semaphore::Semaphore(VkDevice& deviceHandleRef,
    VkSemaphoreCreateFlags semaphoreCreateFlags) :
    deviceHandleRef(deviceHandleRef) {

  this->semaphoreHandle = VK_NULL_HANDLE;

  VkSemaphoreCreateInfo semaphoreCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL,
    .flags = semaphoreCreateFlags
  };

  VkResult result = vkCreateSemaphore(deviceHandleRef, &semaphoreCreateInfo,
      NULL, &this->semaphoreHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateSemaphore");
  }
}

Semaphore::~Semaphore() {
  vkDestroySemaphore(this->deviceHandleRef, this->semaphoreHandle, NULL);
}

VkSemaphore& Semaphore::getSemaphoreHandleRef() {
  return this->semaphoreHandle;
}