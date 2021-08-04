#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"
#include "vrk/device.h"

#include <vulkan/vulkan.h>

#include <vector>

class Image : public Component {
private:
  VkImage imageHandle;

  VkDeviceMemory deviceMemoryHandle;

  VkDevice* deviceHandlePtr;
  VkPhysicalDevice* physicalDeviceHandlePtr;

  VkImageCreateInfo imageCreateInfo;
  VkMemoryPropertyFlags memoryPropertyFlags;
public:
  Image(VkDevice* deviceHandlePtr,
      VkPhysicalDevice* physicalDeviceHandlePtr,
      std::vector<uint32_t>* queueFamilyIndexListPtr,
      VkImageCreateFlags imageCreateFlags,
      VkImageType imageType, VkFormat format, uint32_t width, uint32_t height,
      uint32_t depth, uint32_t mipLevels, uint32_t arrayLayers,
      VkSampleCountFlagBits sampleCountFlagBits, VkImageTiling imageTiling,
      VkImageUsageFlags imageUsageFlags, VkSharingMode sharingMode,
      VkImageLayout initialImageLayout,
      VkMemoryPropertyFlags memoryPropertyFlags);

  ~Image();

  bool activate();

  VkImage* getImageHandlePtr();
};