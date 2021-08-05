#include "vrk/image.h"

Image::Image(VkDevice* deviceHandlePtr,
    VkPhysicalDevice* physicalDeviceHandlePtr,
    uint32_t initialQueueFamilyIndex,
    VkImageCreateFlags imageCreateFlags,
    VkImageType imageType, VkFormat format, uint32_t width, uint32_t height,
    uint32_t depth, uint32_t mipLevels, uint32_t arrayLayers,
    VkSampleCountFlagBits sampleCountFlagBits, VkImageTiling imageTiling,
    VkImageUsageFlags imageUsageFlags, VkSharingMode sharingMode,
    VkImageLayout initialImageLayout, VkMemoryPropertyFlags memoryPropertyFlags)
    : Component("image") {

  this->imageHandle = VK_NULL_HANDLE;

  this->deviceMemoryHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->physicalDeviceHandlePtr = physicalDeviceHandlePtr;

  this->memoryPropertyFlags = memoryPropertyFlags;

  VkExtent3D extent3D = {
    .width = width,
    .height = height,
    .depth = depth
  };

  this->queueFamilyIndexList = {initialQueueFamilyIndex};

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
    .queueFamilyIndexCount = (uint32_t)this->queueFamilyIndexList.size(),
    .pQueueFamilyIndices = this->queueFamilyIndexList.data(),
    .initialLayout = initialImageLayout
  };
}

Image::~Image() {
  vkDestroyImage(*this->deviceHandlePtr, this->imageHandle, NULL);
  vkFreeMemory(*this->deviceHandlePtr, this->deviceMemoryHandle, NULL);
}

void Image::addQueueFamilyIndex(uint32_t queueFamilyIndex) {
  this->queueFamilyIndexList.push_back(queueFamilyIndex);
}

bool Image::activate() {
  if (!Component::activate()) {
    return false;
  }

  this->imageCreateInfo.queueFamilyIndexCount =
      (uint32_t)this->queueFamilyIndexList.size();

  this->imageCreateInfo.pQueueFamilyIndices =
      this->queueFamilyIndexList.data();

  VkResult result = vkCreateImage(*this->deviceHandlePtr,
      &this->imageCreateInfo, NULL, &this->imageHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateImage");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetImageMemoryRequirements(*this->deviceHandlePtr, this->imageHandle,
      &memoryRequirements);

  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties =
      Device::getPhysicalDeviceMemoryProperties(this->physicalDeviceHandlePtr);

  uint32_t memoryTypeIndex = -1;
  for (uint32_t x = 0; x < physicalDeviceMemoryProperties.memoryTypeCount;
      x++) {
    if ((memoryRequirements.memoryTypeBits & (1 << x)) &&
        (physicalDeviceMemoryProperties.memoryTypes[x].propertyFlags &
        this->memoryPropertyFlags) == this->memoryPropertyFlags) {

      memoryTypeIndex = x;
      break;
    }
  }

  VkMemoryAllocateInfo memoryAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = NULL,
    .allocationSize = memoryRequirements.size,
    .memoryTypeIndex = memoryTypeIndex
  };

  result = vkAllocateMemory(*this->deviceHandlePtr, &memoryAllocateInfo, NULL,
      &this->deviceMemoryHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateMemory");
  }

  result = vkBindImageMemory(*this->deviceHandlePtr, this->imageHandle,
      this->deviceMemoryHandle, 0);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkBindImageMemory");
  }

  return true;
}

VkImage* Image::getImageHandlePtr() {
  return &this->imageHandle;
}