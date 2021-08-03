#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class ImageView : public Component {
private:
  VkImageView imageViewHandle;

  VkDevice* deviceHandlePtr;

  VkImageViewCreateInfo imageViewCreateInfo;
public:
  ImageView(VkDevice* deviceHandlePtr, VkImage* imageHandlePtr,
      VkImageViewCreateFlags imageViewCreateFlags,
      VkImageViewType imageViewType, VkFormat format,
      VkComponentSwizzle componentSwizzleR,
      VkComponentSwizzle componentSwizzleG,
      VkComponentSwizzle componentSwizzleB,
      VkComponentSwizzle componentSwizzleA, VkImageAspectFlags imageAspectFlags,
      uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer,
      uint32_t layerCount);

  ~ImageView();

  bool activate();
};