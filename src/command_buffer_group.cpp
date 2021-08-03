#include "vrk/command_buffer_group.h"

CommandBufferGroup::CommandBufferGroup(VkDevice* deviceHandlePtr,
    VkCommandPool* commandPoolHandlePtr,
    VkCommandBufferLevel commandBufferLevel, uint32_t commandBufferCount) :
    Component("command buffer group") {

  this->commandBufferHandleList =
      std::vector<VkCommandBuffer>(commandBufferCount, VK_NULL_HANDLE);

  this->deviceHandlePtr = deviceHandlePtr;

  this->commandPoolHandlePtr = commandPoolHandlePtr;

  this->commandBufferAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL,
    .commandPool = *this->commandPoolHandlePtr,
    .level = commandBufferLevel,
    .commandBufferCount = commandBufferCount
  };
}

CommandBufferGroup::~CommandBufferGroup() {
  vkFreeCommandBuffers(*this->deviceHandlePtr, *this->commandPoolHandlePtr,
      this->commandBufferHandleList.size(),
      this->commandBufferHandleList.data());
}

uint32_t CommandBufferGroup::getCommandBufferCount() {
  return this->commandBufferHandleList.size();
}

bool CommandBufferGroup::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkResult result = vkAllocateCommandBuffers(*this->deviceHandlePtr,
      &this->commandBufferAllocateInfo, this->commandBufferHandleList.data());
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateCommandBuffers");
  }

  return true;
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

  os << static_cast<const Component&>(commandBufferGroup) << std::endl;

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