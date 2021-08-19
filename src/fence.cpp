#include "vrk/fence.h"

Fence::Fence(VkDevice& deviceHandleRef,
    VkFenceCreateFlagBits fenceCreateFlagBits) :
    deviceHandleRef(deviceHandleRef) {

  this->fenceHandle = VK_NULL_HANDLE;

  VkFenceCreateInfo fenceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = NULL,
    .flags = fenceCreateFlagBits
  };

  VkResult result = vkCreateFence(deviceHandleRef, &fenceCreateInfo, NULL,
      &this->fenceHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateFence");
  }
}

Fence::~Fence() {
  vkDestroyFence(this->deviceHandleRef, this->fenceHandle, NULL);
}

bool Fence::getSignaled() {
  VkResult result = vkGetFenceStatus(this->deviceHandleRef, this->fenceHandle);

  if (result != VK_SUCCESS && result != VK_NOT_READY) {
    throwExceptionVulkanAPI(result, "vkGetFenceStatus");
  }

  return result == VK_SUCCESS;
}

bool Fence::waitForSignal(uint32_t timeout) {
  VkResult result = vkWaitForFences(this->deviceHandleRef, 1, 
      &this->fenceHandle, true, timeout);

  if (result != VK_SUCCESS && result != VK_TIMEOUT) {
    throwExceptionVulkanAPI(result, "vkWaitForFences");
  }

  return result == VK_SUCCESS; 
}

void Fence::reset() {
  VkResult result = vkResetFences(this->deviceHandleRef, 1, &this->fenceHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkResetFences");
  }
}

VkFence& Fence::getFenceHandleRef() {
  return this->fenceHandle;
}