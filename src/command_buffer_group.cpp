#include "vrk/command_buffer_group.h"

CommandBufferGroup::CommandBufferGroup(VkDevice& deviceHandleRef,
    VkCommandPool& commandPoolHandleRef,
    VkCommandBufferLevel commandBufferLevel,
    uint32_t commandBufferCount) :
    deviceHandleRef(deviceHandleRef),
    commandPoolHandleRef(commandPoolHandleRef) {

  this->commandBufferHandleList =
      std::vector<VkCommandBuffer>(commandBufferCount, VK_NULL_HANDLE);

  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL,
    .commandPool = commandPoolHandleRef,
    .level = commandBufferLevel,
    .commandBufferCount = commandBufferCount
  };

  VkResult result = vkAllocateCommandBuffers(deviceHandleRef,
      &commandBufferAllocateInfo, this->commandBufferHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateCommandBuffers");
  }
}

CommandBufferGroup::~CommandBufferGroup() {
  vkFreeCommandBuffers(this->deviceHandleRef, this->commandPoolHandleRef,
      this->commandBufferHandleList.size(),
      this->commandBufferHandleList.data());
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

void CommandBufferGroup::submit(
    VkQueue& queueHandleRef,
    std::vector<SubmitInfoParam> submitInfoParamList,
    VkFence& fenceHandleRef) {

  std::vector<VkSubmitInfo> submitInfoList = {};

  for (SubmitInfoParam& submitInfoParam : submitInfoParamList) {
    std::vector<VkCommandBuffer> commandBufferHandleList = {};

    for (uint32_t index : submitInfoParam.commandBufferHandleIndexList) {
      commandBufferHandleList.push_back(this->commandBufferHandleList[index]);
    }

    VkSubmitInfo submitInfo = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = NULL,
      .waitSemaphoreCount =
          (uint32_t)submitInfoParam.waitSemaphoreHandleListRef.size(),
      .pWaitSemaphores = submitInfoParam.waitSemaphoreHandleListRef.data(),
      .pWaitDstStageMask = submitInfoParam.waitPipelineStageFlagsList.data(),
      .commandBufferCount = (uint32_t)commandBufferHandleList.size(),
      .pCommandBuffers = commandBufferHandleList.data(),
      .signalSemaphoreCount =
          (uint32_t)submitInfoParam.signalSemaphoreHandleListRef.size(),
      .pSignalSemaphores = submitInfoParam.signalSemaphoreHandleListRef.data()
    };
  }

  VkResult result = vkQueueSubmit(queueHandleRef,
      (uint32_t)submitInfoList.size(), submitInfoList.data(), fenceHandleRef);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkQueueSubmit");
  }
}