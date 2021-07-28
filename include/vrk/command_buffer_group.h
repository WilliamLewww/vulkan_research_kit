#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class CommandBufferGroup : public Component {
private:
  std::vector<VkCommandBuffer> commandBufferHandleList;

  VkDevice* deviceHandlePtr;
  VkCommandPool* commandPoolHandlePtr;
  VkCommandBufferLevel commandBufferLevel;
public:
  CommandBufferGroup(VkDevice* deviceHandlePtr,
      VkCommandPool* commandPoolHandlePtr,
      VkCommandBufferLevel commandBufferLevel, uint32_t commandBufferCount);

  ~CommandBufferGroup();

  uint32_t getCommandBufferCount();

  bool activate();

  void beginRecording(uint32_t commandBufferIndex,
      VkCommandBufferUsageFlagBits commandBufferUsageFlagBits);

  void endRecording(uint32_t commandBufferIndex);

  void submit(uint32_t commandBufferIndex,
      VkQueue* queueHandlePtr, std::vector<VkSemaphore> waitSemaphoreHandleList,
      std::vector<VkPipelineStageFlags> waitPipelineStageFlagsList,
      std::vector<VkSemaphore> signalSemaphoreHandleList,
      VkFence fenceHandle = VK_NULL_HANDLE);

  friend std::ostream& operator<<(std::ostream& os,
      const CommandBufferGroup& commandBufferGroup);
};