#pragma once

#include "vrk/device.h"
#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class Buffer {
public:
  Buffer(VkDevice &deviceHandleRef, VkPhysicalDevice &physicalDeviceHandleRef,
         VkBufferCreateFlags bufferCreateFlags, VkDeviceSize bufferDeviceSize,
         VkBufferUsageFlags bufferUsageFlags, VkSharingMode sharingMode,
         std::vector<uint32_t> queueFamilyIndexList,
         VkMemoryPropertyFlags memoryPropertyFlags,
         std::vector<void *> memoryAllocateInfoChainList = {});

  ~Buffer();

  void copyFromImageCmd(VkCommandBuffer &commandBufferHandleRef,
                        VkImage &imageHandleRef, VkImageLayout imageLayout,
                        std::vector<VkBufferImageCopy> bufferImageCopyList);

  void copyFromBufferCmd(VkCommandBuffer &commandBufferHandleRef,
                         VkBuffer &srcBufferHandleRef,
                         VkBuffer &dstBufferHandleRef,
                         std::vector<VkBufferCopy> bufferCopyList);

  void mapMemory(void **hostMemoryBuffer, VkDeviceSize offset,
                 VkDeviceSize size);

  void unmapMemory();

  void bindVertexBufferCmd(VkCommandBuffer &commandBufferHandleRef,
                           uint32_t binding);

  void bindIndexBufferCmd(VkCommandBuffer &commandBufferHandleRef,
                          VkIndexType indexType);

  VkDeviceAddress getBufferDeviceAddress();

  VkBuffer &getBufferHandleRef();

private:
  VkBuffer bufferHandle;

  VkDeviceMemory deviceMemoryHandle;

  VkDevice &deviceHandleRef;

  VkDeviceSize bufferDeviceSize;
};
