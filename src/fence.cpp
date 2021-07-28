#include "vrk/fence.h"

Fence::Fence(VkDevice* deviceHandlePtr, VkFenceCreateFlags fenceCreateFlags) :
    Component("Fence") {

  if (*deviceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid device handle");
  }

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

std::ostream& operator<<(std::ostream& os, const Fence& fence) {
  os << static_cast<const Component&>(fence) << std::endl;
  os << "  fence handle: " << fence.fenceHandle << std::endl;
  os << "  device handle (ptr): " << *fence.deviceHandlePtr;

  return os;
}