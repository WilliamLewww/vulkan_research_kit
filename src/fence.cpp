#include "vrk/fence.h"

Fence::Fence(VkDevice* deviceHandlePtr, VkFenceCreateFlags fenceCreateFlags) :
    Component("Fence") {

  this->fenceHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->fenceCreateFlags = fenceCreateFlags;
}

Fence::~Fence() {
  vkDestroyFence(*this->deviceHandlePtr, this->fenceHandle, NULL);
}

bool Fence::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkFenceCreateInfo fenceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = NULL,
    .flags = this->fenceCreateFlags
  };

  VkResult result = vkCreateFence(*this->deviceHandlePtr, &fenceCreateInfo,
      NULL, &this->fenceHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateFence");
  }

  return true;
}

VkFence* Fence::getFenceHandlePtr() {
  return &this->fenceHandle;
}