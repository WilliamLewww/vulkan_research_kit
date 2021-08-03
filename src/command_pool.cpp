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

std::ostream& operator<<(std::ostream& os, const CommandPool& commandPool) {
  os << static_cast<const Component&>(commandPool) << std::endl;
  os << "  command pool handle: " << commandPool.commandPoolHandle << std::endl;
  os << "  device handle (ptr): " << *commandPool.deviceHandlePtr << std::endl;
  os << "  queue family index: " << 
      commandPool.commandPoolCreateInfo.queueFamilyIndex;

  return os;
}