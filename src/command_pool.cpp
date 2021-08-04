#include "vrk/command_pool.h"

CommandPool::CommandPool(VkDevice* deviceHandlePtr, uint32_t queueFamilyIndex) :
    Component("command pool") {

  this->commandPoolHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->commandPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueFamilyIndex = queueFamilyIndex
  };
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(*this->deviceHandlePtr, this->commandPoolHandle, NULL);
}

bool CommandPool::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkResult result = vkCreateCommandPool(*this->deviceHandlePtr,
      &commandPoolCreateInfo, NULL, &this->commandPoolHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateCommandPool");
  }

  return true;
}

void CommandPool::reset(VkCommandPoolResetFlags commandPoolResetFlags) {
  VkResult result = vkResetCommandPool(*this->deviceHandlePtr,
      this->commandPoolHandle, commandPoolResetFlags);
}

VkCommandPool* CommandPool::getCommandPoolHandlePtr() {
  return &this->commandPoolHandle;
}