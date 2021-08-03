#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class Image : public Component {
private:
  VkImage imageHandle;

  VkDevice* deviceHandlePtr;

  VkImageCreateInfo imageCreateInfo;
public:
  Image(VkDevice* deviceHandlePtr,
      std::vector<uint32_t>* queueFamilyIndexListPtr,
      VkImageCreateFlags imageCreateFlags,
      VkImageType imageType, VkFormat format, VkExtent3D extent3D,
      uint32_t mipLevels, uint32_t arrayLayers,
      VkSampleCountFlagBits sampleCountFlagBits, VkImageTiling imageTiling,
      VkImageUsageFlags imageUsageFlags, VkSharingMode sharingMode,
      VkImageLayout initialImageLayout);

  ~Image();

  bool activate();
};