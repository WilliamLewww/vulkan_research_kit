#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class ImageView {
public:
  ImageView(VkDevice &deviceHandleRef, VkImage &imageHandleRef,
            VkImageViewCreateFlags imageViewCreateFlags,
            VkImageViewType imageViewType, VkFormat format,
            VkComponentMapping componentMapping,
            VkImageSubresourceRange imageSubresourceRange);

  ~ImageView();

  VkImageView &getImageViewHandleRef();

private:
  VkImageView imageViewHandle;

  VkDevice &deviceHandleRef;
};