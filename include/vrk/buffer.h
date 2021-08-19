#pragma once

#include "vrk/helper.h"
#include "vrk/device.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

class Buffer {
public:
  Buffer(VkDevice& deviceHandleRef,
      VkPhysicalDevice& physicalDeviceHandleRef,
      VkBufferCreateFlags bufferCreateFlags,
      VkDeviceSize bufferDeviceSize,
      VkBufferUsageFlags bufferUsageFlags,
      VkSharingMode sharingMode,
      std::vector<uint32_t> queueFamilyIndexList,
      VkMemoryPropertyFlags memoryPropertyFlags);

  ~Buffer();

  void copyFromImageCmd(VkCommandBuffer& commandBufferHandleRef,
      VkImage& imageHandleRef,
      VkImageLayout imageLayout,
      std::vector<VkBufferImageCopy> bufferImageCopyList);

  void copyFromBufferCmd(VkCommandBuffer& commandBufferHandleRef,
      VkBuffer& srcBufferHandleRef,
      VkBuffer& dstBufferHandleRef,
      std::vector<VkBufferCopy> bufferCopyList);

  void mapMemory(void** hostMemory,
      VkDeviceSize offset,
      VkDeviceSize size);

  void unmapMemory();

  void bindVertexBufferCmd(VkCommandBuffer& commandBufferHandleRef,
      uint32_t binding);

  void bindIndexBufferCmd(VkCommandBuffer& commandBufferHandleRef,
      VkIndexType indexType);

  VkBuffer& getBufferHandleRef();
private:
  VkBuffer bufferHandle;

  VkDeviceMemory deviceMemoryHandle;

  VkDevice& deviceHandleRef;

  VkDeviceSize bufferDeviceSize;
};