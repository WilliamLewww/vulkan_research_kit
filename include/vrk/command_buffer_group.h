#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class CommandBufferGroup {
public:
  struct SubmitInfoParam {
    std::vector<VkSemaphore>& waitSemaphoreHandleListRef;
    std::vector<VkPipelineStageFlags> waitPipelineStageFlagsList;
    std::vector<uint32_t> commandBufferHandleIndexList;
    std::vector<VkSemaphore>& signalSemaphoreHandleListRef;
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
      VkFence& fenceHandleRef);
private:
  std::vector<VkCommandBuffer> commandBufferHandleList;

  VkDevice& deviceHandleRef;

  VkCommandPool& commandPoolHandleRef;
};