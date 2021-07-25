#include "vrk/command_buffer_group.h"

CommandBufferGroup::CommandBufferGroup(VkDevice* deviceHandlePtr, 
    VkCommandPool* commandPoolHandlePtr, 
    VkCommandBufferLevel commandBufferLevel, uint32_t commandBufferCount) {

  this->isActive = false;

  this->commandBufferHandleList =
      std::vector<VkCommandBuffer>(commandBufferCount, VK_NULL_HANDLE);

  this->deviceHandlePtr = deviceHandlePtr;
  this->commandPoolHandlePtr = commandPoolHandlePtr;
  this->commandBufferLevel = commandBufferLevel;
}

CommandBufferGroup::~CommandBufferGroup() {

}

void CommandBufferGroup::activate() {
  if (this->isActive) {
    PRINT_MESSAGE(std::cerr, "Command buffer group is already active");
    return;
  }

  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL,
    .commandPool = *this->commandPoolHandlePtr,
    .level = this->commandBufferLevel,
    .commandBufferCount = (uint32_t)this->commandBufferHandleList.size()
  };

  VkResult result = vkAllocateCommandBuffers(*this->deviceHandlePtr, 
      &commandBufferAllocateInfo, this->commandBufferHandleList.data());
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateCommandBuffers");
  }

  this->isActive = true;
}

std::ostream& operator<<(std::ostream& os,
    const CommandBufferGroup& commandBufferGroup) {

  os << "command buffer group: " << &commandBufferGroup << std::endl;

  for (uint32_t x = 0; x < commandBufferGroup.commandBufferHandleList.size();
      x++) {

    os << "  command buffer handle " << x << ": " << 
        commandBufferGroup.commandBufferHandleList[x] << std::endl;
  }

  os << "  device handle (ptr): " << *commandBufferGroup.deviceHandlePtr << 
      std::endl;

  os << "  command pool handle (ptr): " << 
      *commandBufferGroup.commandPoolHandlePtr;

  return os;
}