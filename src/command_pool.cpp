#include "vrk/command_pool.h"

CommandPool::CommandPool(VkDevice& deviceHandleRef,
    VkCommandPoolCreateFlags commandPoolCreateFlags,
    uint32_t queueFamilyIndex) :
    deviceHandleRef(deviceHandleRef) {

  this->commandPoolHandle = VK_NULL_HANDLE;

  VkCommandPoolCreateInfo commandPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = commandPoolCreateFlags,
    .queueFamilyIndex = queueFamilyIndex
  };

  VkResult result = vkCreateCommandPool(this->deviceHandleRef,
      &commandPoolCreateInfo, NULL, &this->commandPoolHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateCommandPool");
  }
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(this->deviceHandleRef, this->commandPoolHandle, NULL);
}

void CommandPool::reset(VkCommandPoolResetFlags commandPoolResetFlags) {
  VkResult result = vkResetCommandPool(this->deviceHandleRef,
      this->commandPoolHandle, commandPoolResetFlags);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkResetCommandPool");
  }
}

VkCommandPool& CommandPool::getCommandPoolHandleRef() {
  return this->commandPoolHandle;
}