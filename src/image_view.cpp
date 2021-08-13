#include "vrk/image_view.h"

ImageView::ImageView(VkDevice& deviceHandleRef,
    VkImage& imageHandleRef,
    VkImageViewCreateFlags imageViewCreateFlags,
    VkImageViewType imageViewType,
    VkFormat format,
    VkComponentMapping componentMapping,
    VkImageSubresourceRange imageSubresourceRange) :
    deviceHandleRef(deviceHandleRef) {

  this->imageViewHandle = VK_NULL_HANDLE;

  VkImageViewCreateInfo imageViewCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = NULL,
    .flags = imageViewCreateFlags,
    .image = imageHandleRef,
    .viewType = imageViewType,
    .format = format,
    .components = componentMapping,
    .subresourceRange = imageSubresourceRange
  };

  VkResult result = vkCreateImageView(deviceHandleRef,
      &imageViewCreateInfo, NULL, &this->imageViewHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateImageView");
  }
}

ImageView::~ImageView() {
  vkDestroyImageView(this->deviceHandleRef, this->imageViewHandle, NULL);
}