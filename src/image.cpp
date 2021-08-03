#include "vrk/image.h"

Image::Image(VkDevice* deviceHandlePtr,
    std::vector<uint32_t>* queueFamilyIndexListPtr,
    VkImageCreateFlags imageCreateFlags,
    VkImageType imageType, VkFormat format, VkExtent3D extent3D,
    uint32_t mipLevels, uint32_t arrayLayers,
    VkSampleCountFlagBits sampleCountFlagBits, VkImageTiling imageTiling,
    VkImageUsageFlags imageUsageFlags, VkSharingMode sharingMode,
    VkImageLayout initialImageLayout) : Component("image") {

  this->imageHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->imageCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = NULL,
    .flags = imageCreateFlags,
    .imageType = imageType,
    .format = format,
    .extent = extent3D,
    .mipLevels = mipLevels,
    .arrayLayers = arrayLayers,
    .samples = sampleCountFlagBits,
    .tiling = imageTiling,
    .usage = imageUsageFlags,
    .sharingMode = sharingMode,
    .queueFamilyIndexCount = (uint32_t)queueFamilyIndexListPtr->size(),
    .pQueueFamilyIndices = queueFamilyIndexListPtr->data(),
    .initialLayout = initialImageLayout
  };
}

Image::~Image() {
  vkDestroyImage(*this->deviceHandlePtr, this->imageHandle, NULL);
}

bool Image::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkResult result = vkCreateImage(*this->deviceHandlePtr,
      &this->imageCreateInfo, NULL, &this->imageHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateImage");
  }

  return true;
}