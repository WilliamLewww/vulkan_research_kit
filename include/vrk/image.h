#pragma once

#include "vrk/helper.h"
#include "vrk/device.h"

#include <vulkan/vulkan.h>

#include <vector>

class Image {
public:
  Image(VkDevice& deviceHandleRef,
      VkPhysicalDevice& physicalDeviceHandleRef,
      VkImageCreateFlags imageCreateFlags,
      VkImageType imageType,
      VkFormat format,
      VkExtent3D extent3D,
      uint32_t mipLevels,
      uint32_t arrayLayers,
      VkSampleCountFlagBits sampleCountFlagBits,
      VkImageTiling imageTiling,
      VkImageUsageFlags imageUsageFlags,
      VkSharingMode sharingMode,
      std::vector<uint32_t> queueFamilyIndexList,
      VkImageLayout initialImageLayout,
      VkMemoryPropertyFlags memoryPropertyFlags);

  ~Image();
private:
  VkImage imageHandle;

  VkDeviceMemory deviceMemoryHandle;

  VkDevice& deviceHandleRef;
};