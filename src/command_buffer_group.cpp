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
    VkFence fenceHandle) {

  std::vector<VkSubmitInfo> submitInfoList = {};
  std::vector<std::vector<VkCommandBuffer>> commandBufferHandleListSubmit = {};

  for (SubmitInfoParam& submitInfoParam : submitInfoParamList) {
    std::vector<VkCommandBuffer> commandBufferHandleListLocal = {};

    for (uint32_t index : submitInfoParam.commandBufferHandleIndexList) {
      commandBufferHandleListLocal.push_back(
          this->commandBufferHandleList[index]);
    }
    commandBufferHandleListSubmit.push_back(commandBufferHandleListLocal);

    VkSubmitInfo submitInfo = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = NULL,
      .waitSemaphoreCount =
          (uint32_t)submitInfoParam.waitSemaphoreHandleList.size(),
      .pWaitSemaphores = submitInfoParam.waitSemaphoreHandleList.data(),
      .pWaitDstStageMask = submitInfoParam.waitPipelineStageFlagsList.data(),
      .commandBufferCount = (uint32_t)commandBufferHandleListSubmit[
          commandBufferHandleListSubmit.size() - 1].size(),
      .pCommandBuffers = commandBufferHandleListSubmit[
          commandBufferHandleListSubmit.size() - 1].data(),
      .signalSemaphoreCount =
          (uint32_t)submitInfoParam.signalSemaphoreHandleList.size(),
      .pSignalSemaphores = submitInfoParam.signalSemaphoreHandleList.data()
    };

    submitInfoList.push_back(submitInfo);
  }

  VkResult result = vkQueueSubmit(queueHandleRef,
      (uint32_t)submitInfoList.size(), submitInfoList.data(), fenceHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkQueueSubmit");
  }
}

void CommandBufferGroup::reset(uint32_t commandBufferIndex,
      VkCommandBufferResetFlags commandBufferResetFlags) {

  VkResult result = vkResetCommandBuffer(this->commandBufferHandleList[
      commandBufferIndex],
      commandBufferResetFlags);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkResetCommandBuffer");
  }
}

void CommandBufferGroup::createPipelineBarrierCmd(uint32_t commandBufferIndex,
    VkPipelineStageFlags srcPipelineStageFlags,
    VkPipelineStageFlags dstPipelineStageFlags,
    VkDependencyFlags dependencyFlags,
    std::vector<MemoryBarrierParam> memoryBarrierParamList,
    std::vector<BufferMemoryBarrierParam> bufferMemoryBarrierParamList,
    std::vector<ImageMemoryBarrierParam> imageMemoryBarrierParamList) {

  std::vector<VkMemoryBarrier> memoryBarrierList = {};
  for (MemoryBarrierParam memoryBarrierParam : memoryBarrierParamList) {
    VkMemoryBarrier memoryBarrier = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
      .pNext = NULL,
      .srcAccessMask = memoryBarrierParam.srcMaskAccessFlags,
      .dstAccessMask = memoryBarrierParam.dstMaskAccessFlags
    };

    memoryBarrierList.push_back(memoryBarrier);
  }

  std::vector<VkBufferMemoryBarrier> bufferMemoryBarrierList = {};
  for (BufferMemoryBarrierParam bufferMemoryBarrierParam :
      bufferMemoryBarrierParamList) {

    VkBufferMemoryBarrier bufferMemoryBarrier = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .pNext = NULL,
      .srcAccessMask = bufferMemoryBarrierParam.srcMaskAccessFlags,
      .dstAccessMask = bufferMemoryBarrierParam.dstMaskAccessFlags,
      .srcQueueFamilyIndex = bufferMemoryBarrierParam.srcQueueFamilyIndex,
      .dstQueueFamilyIndex = bufferMemoryBarrierParam.dstQueueFamilyIndex,
      .buffer = bufferMemoryBarrierParam.bufferHandleRef,
      .offset = bufferMemoryBarrierParam.offsetDeviceSize,
      .size = bufferMemoryBarrierParam.sizeDeviceSize
    };

    bufferMemoryBarrierList.push_back(bufferMemoryBarrier);
  }

  std::vector<VkImageMemoryBarrier> imageMemoryBarrierList = {};
  for (ImageMemoryBarrierParam imageMemoryBarrierParam :
      imageMemoryBarrierParamList) {

    VkImageMemoryBarrier imageMemoryBarrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = NULL,
      .srcAccessMask = imageMemoryBarrierParam.srcMaskAccessFlags,
      .dstAccessMask = imageMemoryBarrierParam.dstMaskAccessFlags,
      .oldLayout = imageMemoryBarrierParam.oldImageLayout,
      .newLayout = imageMemoryBarrierParam.newImageLayout,
      .srcQueueFamilyIndex = imageMemoryBarrierParam.srcQueueFamilyIndex,
      .dstQueueFamilyIndex = imageMemoryBarrierParam.dstQueueFamilyIndex,
      .image = imageMemoryBarrierParam.imageHandleRef,
      .subresourceRange = imageMemoryBarrierParam.imageSubresourceRange
    };

    imageMemoryBarrierList.push_back(imageMemoryBarrier);
  }

  vkCmdPipelineBarrier(this->commandBufferHandleList[commandBufferIndex],
      srcPipelineStageFlags,
      dstPipelineStageFlags,
      dependencyFlags,
      (uint32_t)memoryBarrierList.size(),
      memoryBarrierList.data(),
      (uint32_t)bufferMemoryBarrierList.size(),
      bufferMemoryBarrierList.data(),
      (uint32_t)imageMemoryBarrierList.size(),
      imageMemoryBarrierList.data());
}

VkCommandBuffer& CommandBufferGroup::getCommandBufferHandleRef(
    uint32_t index) {

  return this->commandBufferHandleList[index];
}
