#include "vrk/command_pool.h"

CommandPool::CommandPool(VkDevice* deviceHandlePtr, uint32_t queueFamilyIndex) {
  this->isActive = false;

  this->commandPoolHandle = VK_NULL_HANDLE;
  this->deviceHandlePtr = deviceHandlePtr;

  this->queueFamilyIndex = queueFamilyIndex;
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(*this->deviceHandlePtr, this->commandPoolHandle, NULL);
}

void CommandPool::activate() {
  if (this->isActive) {
    PRINT_MESSAGE(std::cerr, "Command pool is already active");
    return;
  }

  VkCommandPoolCreateInfo commandPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueFamilyIndex = this->queueFamilyIndex
  };

  VkResult result = vkCreateCommandPool(*this->deviceHandlePtr,
      &commandPoolCreateInfo, NULL, &this->commandPoolHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateCommandPool");
  }

  this->isActive = true;
}

void CommandPool::reset(VkCommandPoolResetFlags commandPoolResetFlags) {
  VkResult result = vkResetCommandPool(*this->deviceHandlePtr, 
      this->commandPoolHandle, commandPoolResetFlags);
}

VkCommandPool* CommandPool::getCommandPoolHandlePtr() {
  return &this->commandPoolHandle;
}

std::ostream& operator<<(std::ostream& os, const CommandPool& commandPool) {
  os << "command pool: " << &commandPool << std::endl;
  os << "  command pool handle: " << commandPool.commandPoolHandle << std::endl;
  os << "  device handle (ptr): " << *commandPool.deviceHandlePtr << std::endl;
  os << "  queue family index: " << commandPool.queueFamilyIndex;

  return os;
}