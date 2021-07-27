#include "vrk/command_buffer_group.h"

CommandBufferGroup::CommandBufferGroup(VkDevice* deviceHandlePtr,
    VkCommandPool* commandPoolHandlePtr,
    VkCommandBufferLevel commandBufferLevel, uint32_t commandBufferCount) {

  if (*deviceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid device handle");
  }

  if (*commandPoolHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid command pool handle");
  }

  this->isActive = false;

  this->commandBufferHandleList =
      std::vector<VkCommandBuffer>(commandBufferCount, VK_NULL_HANDLE);

  this->deviceHandlePtr = deviceHandlePtr;
  this->commandPoolHandlePtr = commandPoolHandlePtr;
  this->commandBufferLevel = commandBufferLevel;
}

CommandBufferGroup::~CommandBufferGroup() {
  vkFreeCommandBuffers(*this->deviceHandlePtr, *this->commandPoolHandlePtr,
      this->commandBufferHandleList.size(),
      this->commandBufferHandleList.data());
}

uint32_t CommandBufferGroup::getCommandBufferCount() {
  return this->commandBufferHandleList.size();
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

void CommandBufferGroup::beginRecording(uint32_t commandBufferIndex,
    VkCommandBufferUsageFlagBits commandBufferUsageFlagBits) {

  VkCommandBufferBeginInfo commandBufferBeginInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = NULL,
    .flags = commandBufferUsageFlagBits,
    .pInheritanceInfo = NULL
  };

  VkResult result = vkBeginCommandBuffer(
      this->commandBufferHandleList[commandBufferIndex],
      &commandBufferBeginInfo);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkBeginCommandBuffer");
  }
}

void CommandBufferGroup::endRecording(uint32_t commandBufferIndex) {
  VkResult result = vkEndCommandBuffer(
      this->commandBufferHandleList[commandBufferIndex]);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEndCommandBuffer");
  }
}

void CommandBufferGroup::submit(uint32_t commandBufferIndex,
    VkQueue* queueHandlePtr, std::vector<VkSemaphore> waitSemaphoreHandleList,
    std::vector<VkPipelineStageFlags> waitPipelineStageFlagsList,
    std::vector<VkSemaphore> signalSemaphoreHandleList, VkFence fenceHandle) {

  if (*queueHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid queue handle");
  }

  for (VkSemaphore semaphoreHandle : waitSemaphoreHandleList) {
    if (semaphoreHandle == VK_NULL_HANDLE) {
      throwExceptionMessage("Invalid semaphore handle");
    }
  }

  for (VkSemaphore semaphoreHandle : signalSemaphoreHandleList) {
    if (semaphoreHandle == VK_NULL_HANDLE) {
      throwExceptionMessage("Invalid semaphore handle");
    }
  }

  VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = NULL,
    .waitSemaphoreCount = (uint32_t)waitSemaphoreHandleList.size(),
    .pWaitSemaphores = waitSemaphoreHandleList.data(),
    .pWaitDstStageMask = waitPipelineStageFlagsList.data(),
    .commandBufferCount = 1,
    .pCommandBuffers = &this->commandBufferHandleList[commandBufferIndex],
    .signalSemaphoreCount = (uint32_t)signalSemaphoreHandleList.size(),
    .pSignalSemaphores = signalSemaphoreHandleList.data()
  };

  VkResult result = vkQueueSubmit(*queueHandlePtr, 1, &submitInfo, fenceHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkQueueSubmit");
  }
}

std::ostream& operator<<(std::ostream& os,
    const CommandBufferGroup& commandBufferGroup) {

  std::string activeMessage = (commandBufferGroup.isActive) ?
      "active" : "inactive";

  os << "command buffer group " << "(" << activeMessage << "): " <<
      &commandBufferGroup << std::endl;

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