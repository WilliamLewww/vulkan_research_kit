#pragma once

#include "vrk/device.h"
#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class Image {
public:
  Image(VkDevice &deviceHandleRef, VkPhysicalDevice &physicalDeviceHandleRef,
        VkImageCreateFlags imageCreateFlags, VkImageType imageType,
        VkFormat format, VkExtent3D extent3D, uint32_t mipLevels,
        uint32_t arrayLayers, VkSampleCountFlagBits sampleCountFlagBits,
        VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags,
        VkSharingMode sharingMode, std::vector<uint32_t> queueFamilyIndexList,
        VkImageLayout initialImageLayout,
        VkMemoryPropertyFlags memoryPropertyFlags);

  ~Image();

  void copyFromBufferCmd(VkCommandBuffer &commandBufferHandleRef,
                         VkBuffer &srcBufferHandleRef,
                         VkImageLayout imageLayout,
                         std::vector<VkBufferImageCopy> bufferImageCopyList);

  void copyFromImageCmd(VkCommandBuffer &commandBufferHandleRef,
                        VkImage &srcImageHandleRef,
                        VkImageLayout srcImageLayout,
                        VkImageLayout dstImageLayout,
                        std::vector<VkImageCopy> imageCopyList);

  VkImage &getImageHandleRef();

  void mapMemory(void **hostMemory, VkDeviceSize offset, VkDeviceSize size);

  void unmapMemory();

private:
  VkImage imageHandle;

  VkDeviceMemory deviceMemoryHandle;

  VkDevice &deviceHandleRef;
};
