#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class CommandBufferGroup {
private:
  bool isActive;

  std::vector<VkCommandBuffer> commandBufferHandleList;

  VkDevice* deviceHandlePtr;
  VkCommandPool* commandPoolHandlePtr;
  VkCommandBufferLevel commandBufferLevel;
public:
  CommandBufferGroup(VkDevice* deviceHandlePtr, 
      VkCommandPool* commandPoolHandlePtr, 
      VkCommandBufferLevel commandBufferLevel, uint32_t commandBufferCount);

  ~CommandBufferGroup();

  void activate();

  friend std::ostream& operator<<(std::ostream& os,
      const CommandBufferGroup& commandBufferGroup);
};