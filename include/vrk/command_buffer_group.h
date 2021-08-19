#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class CommandBufferGroup {
public:
  struct SubmitInfoParam {
    std::vector<VkSemaphore> waitSemaphoreHandleList;
    std::vector<VkPipelineStageFlags> waitPipelineStageFlagsList;
    std::vector<uint32_t> commandBufferHandleIndexList;
    std::vector<VkSemaphore> signalSemaphoreHandleList;
  };

  struct MemoryBarrierParam {
    VkAccessFlags srcMaskAccessFlags;
    VkAccessFlags dstMaskAccessFlags;
  };

  struct BufferMemoryBarrierParam {
    VkAccessFlags srcMaskAccessFlags;
    VkAccessFlags dstMaskAccessFlags;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
    VkBuffer& bufferHandleRef;
    VkDeviceSize offsetDeviceSize;
    VkDeviceSize sizeDeviceSize;
  };

  struct ImageMemoryBarrierParam {
    VkAccessFlags srcMaskAccessFlags;
    VkAccessFlags dstMaskAccessFlags;
    VkImageLayout oldImageLayout;
    VkImageLayout newImageLayout;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
    VkImage& imageHandleRef;
    VkImageSubresourceRange imageSubresourceRange;
  };

  CommandBufferGroup(VkDevice& deviceHandleRef,
      VkCommandPool& commandPoolHandleRef,
      VkCommandBufferLevel commandBufferLevel,
      uint32_t commandBufferCount);

  ~CommandBufferGroup();

  void beginRecording(uint32_t commandBufferIndex,
      VkCommandBufferUsageFlagBits commandBufferUsageFlagBits);

  void endRecording(uint32_t commandBufferIndex);

  void submit(VkQueue& queueHandleRef,
      std::vector<SubmitInfoParam> submitInfoParamList,
      VkFence fenceHandle);

  void createPipelineBarrierCmd(uint32_t commandBufferIndex,
      VkPipelineStageFlags srcPipelineStageFlags,
      VkPipelineStageFlags dstPipelineStageFlags,
      VkDependencyFlags dependencyFlags,
      std::vector<MemoryBarrierParam> memoryBarrierParamList,
      std::vector<BufferMemoryBarrierParam> bufferMemoryBarrierParamList,
      std::vector<ImageMemoryBarrierParam> imageMemoryBarrierParamList);

  VkCommandBuffer& getCommandBufferHandleRef(uint32_t index);
private:
  std::vector<VkCommandBuffer> commandBufferHandleList;

  VkDevice& deviceHandleRef;

  VkCommandPool& commandPoolHandleRef;
};