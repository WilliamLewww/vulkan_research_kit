#include "vrk/image_view.h"

ImageView::ImageView(VkDevice* deviceHandlePtr, VkImage* imageHandlePtr,
    VkImageViewCreateFlags imageViewCreateFlags, VkImageViewType imageViewType,
    VkFormat format, VkComponentSwizzle componentSwizzleR,
    VkComponentSwizzle componentSwizzleG, VkComponentSwizzle componentSwizzleB,
    VkComponentSwizzle componentSwizzleA, VkImageAspectFlags imageAspectFlags,
    uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer,
    uint32_t layerCount) : Component("image view") {

  this->imageViewHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->imageHandlePtr = imageHandlePtr;

  VkComponentMapping componentMapping = {
    .r = componentSwizzleR,
    .g = componentSwizzleG,
    .b = componentSwizzleB,
    .a = componentSwizzleA
  };

  VkImageSubresourceRange imageSubresourceRange = {
    .aspectMask = imageAspectFlags,
    .baseMipLevel = baseMipLevel,
    .levelCount = levelCount,
    .baseArrayLayer = baseArrayLayer,
    .layerCount = layerCount
  };

  this->imageViewCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = NULL,
    .flags = imageViewCreateFlags,
    .image = *this->imageHandlePtr,
    .viewType = imageViewType,
    .format = format,
    .components = componentMapping,
    .subresourceRange = imageSubresourceRange
  };
}

ImageView::~ImageView() {

}

bool ImageView::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkResult result = vkCreateImageView(*this->deviceHandlePtr,
      &this->imageViewCreateInfo, NULL, &this->imageViewHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateImageView");
  }

  return true;
}